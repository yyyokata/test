import torch
import numpy as np
in0=torch.arange(1,24,1)
print(in0)
in1=in0.reshape(1,2,2,6)
in1=in1.reshape(1,2,3,4)
print(in1)
in2=in0.reshape(1,2,2,6)
in2=in2.permute(0,2,3,1)
in2=in0.reshape(1,3,4,2)
in2=in2.permute(0,3,1,2)
print(in2)
