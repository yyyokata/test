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

#include "magicmind/cc/interface_builder.h"
#include "magicmind/cc/interface_network.h"
#include "magicmind/cc/interface_runtime.h"
#include "magicmind/common/common.h"
#include "magicmind/utils/status_util.h"

using namespace magicmind;

int main(int argc, char **argv) {
  IBuilder *builder = CreateIBuilder();
  IBuilderConfig *builder_config = CreateIBuilderConfig();
  INetwork *network = CreateINetwork();

  std::vector<DataType> input_dtype({DataType::INT8});
  std::vector<Dims> input_dims({Dims({1, 3, 3, 2})});
  DataType filter_dtype = DataType::INT8;
  Dims filter_dims({1, 3, 3, 2});
  DataType bias_dtype = DataType::FLOAT32;
  Dims bias_dims({1});

  int8_t filter_ptr[filter_dims.GetElementCount()] = {-127, -54, 99,  -9, -46, -7,  -82, 120, 123,
                                                      -34,  22,  123, 70, 93,  -42, -65, 71,  -78};
  float bias_ptr[bias_dims.GetElementCount()] = {0};

  ITensor *input_tensor = network->AddInput(input_dtype[0], input_dims.at(0));
  IConstNode *filter = network->AddIConstNode(filter_dtype, filter_dims, filter_ptr);
  IConstNode *bias = network->AddIConstNode(bias_dtype, bias_dims, bias_ptr);
  IConvNode *conv1 = network->AddIConvNode(input_tensor, filter->GetOutput(0), bias->GetOutput(0));

  conv1->SetStride(2, 2);
  conv1->SetPad(1, 1, 1, 1);
  conv1->SetDilation(1, 1);

  auto filter_tensor = filter->GetOutput(0);

  QuantizationParam input_param = {-7, 0.992544830, 0};
  QuantizationParam filter_param = {-7, 0.992575884, 0};

  input_tensor->SetQuantizationParam(input_param, true);
  filter_tensor->SetQuantizationParam(filter_param, true);

  auto output_tensor = conv1->GetOutput(0);
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
  model->Destroy();
  return 0;
}
