/*************************************************************************
 * Copyright (C) [2020-2023] by Cambricon, Inc.
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *************************************************************************/
#include <utility>
#include <functional>

#include "cnrt.h"  // NOLINT
#include "magicmind/common/common.h"
#include "magicmind/utils/status_util.h"
#include "magicmind/cc/interface_builder.h"
#include "magicmind/cc/interface_network.h"
#include "magicmind/cc/interface_runtime.h"

using namespace magicmind;  // NOLINT

int main(int argc, char **argv) {
  IBuilder *builder = CreateIBuilder();
  IBuilderConfig *builder_config = CreateIBuilderConfig();
  INetwork *network = CreateINetwork();

  std::vector<DataType> input_dtype({DataType::FLOAT16});
  std::vector<Dims> input_dims({Dims({1, 3, 3, 2})});
  DataType filter_dtype = DataType::FLOAT16;
  Dims filter_dims({1, 3, 3, 2});
  DataType bias_dtype = DataType::FLOAT16;
  Dims bias_dims({1});

  uint16_t filter_ptr[filter_dims.GetElementCount()] = {47104, 47104, 47104, 47104, 47104, 47104,
                                                        47104, 47104, 47104, 47104, 47104, 47104,
                                                        47104, 47104, 47104, 47104, 47104, 47104};
  uint16_t bias_ptr[bias_dims.GetElementCount()] = {0};

  ITensor *input_tensor = network->AddInput(input_dtype[0], input_dims.at(0));
  IConstNode *filter = network->AddIConstNode(filter_dtype, filter_dims, filter_ptr);
  IConstNode *bias = network->AddIConstNode(bias_dtype, bias_dims, bias_ptr);
  IConvNode *conv1 = network->AddIConvNode(input_tensor, filter->GetOutput(0), bias->GetOutput(0));

  conv1->SetStride(2, 2);
  conv1->SetPad(1, 1, 1, 1);
  conv1->SetDilation(1, 1);

  auto filter_tensor = filter->GetOutput(0);

  //  QuantizationParam input_param = {-7, 0.992544830, 0};
  //  QuantizationParam filter_param = {-7, 0.992575884, 0};

  //  input_tensor->SetQuantizationParam(input_param, true);
  //  filter_tensor->SetQuantizationParam(filter_param, true);

  auto output_tensor = conv1->GetOutput(0);
  output_tensor->SetDataType(DataType::FLOAT16);
  network->MarkOutput(output_tensor);

  IModel *model = builder->BuildModel("test_model", network, builder_config);
  if (model == nullptr) {
    LOG(ERROR) << "Build Model failed!";
    return 1;
  }

  if (argc == 3) {
    model->SerializeToFile(argv[1], argv[2]);
  }

  network->Destroy();
  builder_config->Destroy();
  builder->Destroy();

  cnrtDev_t dev;
  cnrtInit(0);
  cnrtGetDeviceHandle(&dev, 0);
  cnrtSetCurrentDevice(dev);

  cnrtQueue_t queue;
  cnrtCreateQueue(&queue);

  IEngine *engine = model->CreateEngine();
  if (!engine) {
    LOG(ERROR) << "Create Engine failed.";
    return 1;
  }
  IContext *context = engine->CreateContext();
  if (!context) {
    LOG(ERROR) << "Create Context failed.";
    return 1;
  }

  std::vector<IRTTensor *> inputs, outputs(0);

  if (CreateInputTensors(context, &inputs) != Status::OK()) {
    LOG(ERROR) << "Failed to create input tensors from model.";
    return 0;
  }
  if (CreateOutputTensors(context, &outputs) != Status::OK()) {
    LOG(ERROR) << "Failed to create output tensors from model.";
    return 0;
  }
  uint16_t input[1 * 3 * 3 * 2] = {47104, 47104, 47104, 47104, 47104, 47104, 47104, 47104, 47104,
                                   47104, 47104, 47104, 47104, 47104, 47104, 47104, 47104, 47104};
  for (uint32_t i = 0; i < inputs.size(); i++) {
    inputs[i]->SetDimensions(magicmind::Dims({1, 3, 3, 2}));
    void *dev;
    cnrtMalloc(&dev, inputs[i]->GetSize());
    cnrtMemcpy(dev, input, inputs[i]->GetSize(), CNRT_MEM_TRANS_DIR_HOST2DEV);
    inputs[i]->SetData(dev);
  }
  context->InferOutputShape(inputs, outputs);
  for (uint32_t i = 0; i < inputs.size(); i++) {
    void *dev;
    cnrtMalloc(&dev, outputs[i]->GetSize());
    outputs[i]->SetData(dev);
  }
  // add event
  cnrtNotifier_t event_start;
  cnrtNotifier_t event_end;
  cnrtCreateNotifier(&event_start);
  cnrtCreateNotifier(&event_end);

  // start event
  cnrtPlaceNotifier(event_start, queue);
  magicmind::Status status = context->Enqueue(inputs, outputs, queue);
  if (status != Status::OK()) {
    LOG(ERROR) << "Context Enqueue failed.";
    return 1;
  }
  // end event
  cnrtPlaceNotifier(event_end, queue);
  cnrtSyncQueue(queue);
  float ptv;
  cnrtNotifierDuration(event_start, event_end, &ptv);
  cnrtDestroyNotifier(&event_start);
  cnrtDestroyNotifier(&event_end);
  LOG(INFO) << "Enqueue notifier time " << ptv << " um.";

  uint16_t host_ptr[4];
  cnrtMemcpy(host_ptr, outputs[0]->GetMutableData(), outputs[0]->GetSize(),
             CNRT_MEM_TRANS_DIR_DEV2HOST);
  for (int i = 0; i < 4; ++i) {
    std::cout << host_ptr[i] << "\n";
  }

  for (auto tensor : inputs) {
    tensor->Destroy();
  }
  for (auto tensor : outputs) {
    tensor->Destroy();
  }
  context->Destroy();
  engine->Destroy();
  model->Destroy();
  return 0;
}
