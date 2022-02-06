import tensorflow as tf
import numpy as np
xin=np.empty([1,0,3,1])
zin=np.empty([3,3,1,1])
x=tf.constant(xin)
z=tf.constant(zin)
r=tf.nn.conv2d(x,z,strides=[1,2,2,1],padding='SAME')
print(r)


