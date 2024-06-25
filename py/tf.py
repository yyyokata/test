import tensorflow as tf
import numpy as np
with tf.compat.v1.Session():
  xin=np.empty([1,0,3,1])
  zin=np.empty([3,3,1,1])
  x=tf.constant(xin, name = "a")
  z=tf.constant(zin, name = "a")
  r=tf.nn.conv2d(x,z,strides=[1,2,2,1],padding='SAME')
  print(r)
  
  print(z.name)

