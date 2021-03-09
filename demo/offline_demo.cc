/*************************************************************************
 * Copyright (C) [2020-2023] by Cambricon, Inc.
 *
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
#include <ctime>
#include <iostream>
#include <string>
#include <memory>
#include <fstream>
#include <vector>

#include "cnrt.h"
#include "magicmind/cc/interface_runtime.h"
#include "magicmind/cc/interface_plugin.h"
#include "magicmind/utils/logging.h"
#include "magicmind/utils/status_util.h"

// Bazel create model in bazel-bin/magicmind/test
const char *graph_file = "magicmind/test/conv.graph";
const char *data_file = "magicmind/test/conv.data";

using namespace magicmind;

int main() {
  cnrtDev_t dev;
  cnrtInit(0);
  cnrtGetDeviceHandle(&dev, 0);
  cnrtSetCurrentDevice(dev);

  cnrtQueue_t queue;
  cnrtCreateQueue(&queue);

  IModel *model = CreateModelHandle();
  model->DeserializeFromFile(graph_file, data_file);

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
    LOG(ERROR) << "Failed to create input tensor.";
    return 0;
  }
  int8_t input[2][1 * 3 * 3 * 2] = {{5, 1, 8, 1, 6, 4, 3, 8, 2, 6, 0, 6, 8, 5, 7, 4, 9, 6},
                                    {1, 5, 7, 5, 4, 2, 1, 8, 3, 8, 6, 2, 4, 8, 5, 0, 9, 5}};
  for (uint32_t i = 0; i < inputs.size(); i++) {
    inputs[i]->SetDimensions(magicmind::Dims({1, 3, 3, 2}));
    void *dev;
    cnrtMalloc(&dev, inputs[i]->GetSize());
    cnrtMemcpy(dev, input[i], inputs[i]->GetSize(), CNRT_MEM_TRANS_DIR_HOST2DEV);
    inputs[i]->SetData(dev);
  }
  // add event
  cnrtNotifier_t event_start;
  cnrtNotifier_t event_end;
  cnrtCreateNotifier(&event_start);
  cnrtCreateNotifier(&event_end);

  // start event
  cnrtPlaceNotifier(event_start, queue);
  magicmind::Status status = context->Enqueue(inputs, &outputs, queue);
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

  float host_ptr[4];
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
