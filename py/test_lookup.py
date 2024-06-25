import tensorflow as tf

with tf.compat.v1.Session():
  init1 = tf.compat.v1.lookup.KeyValueTensorInitializer(['a', 'b'], [1, 2])
  table1 = tf.compat.v1.lookup.StaticHashTable(init1, default_value=-1)
  init = tf.compat.v1.lookup.KeyValueTensorInitializer(['a', 'b'], [1, 2])
  table = tf.compat.v1.lookup.StaticHashTable(init, default_value=-1)
  init_op = tf.compat.v1.tables_initializer()
  with tf.control_dependencies([init_op]):
    result = table.lookup(tf.constant(['a', 'c'])).eval()
    result2 = table1.lookup(tf.constant(['a', 'c'])).eval()
  add = tf.add(result, result2)
  print(add)
  res = add.eval()
print(res)
