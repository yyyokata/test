import torch
import numpy as np
m=torch.nn.Conv2d(1,1,3,stride=2,padding=(3,3))
input=torch.rand(1,1,0,0)
print(input)
output=m(input)
print(output)
