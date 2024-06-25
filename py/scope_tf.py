import tensorflow as tf
import numpy as np

@contextlib.contextmanager
def test_scope(key, is_fallback = True):
  def test_callable(node_def):
    if not is_fallback:
      return attr_value_pb2.AttrValue(b = False)
    # if not a fall back, record node def here.
    recorded_nodes = ops.get_collection("_test" + key)
    if not recorded_nodes:
      recorded_nodes = [node_def]
      ops.add_to_collection("_test" + key, node_def)
    else:
      recorded_nodes.append(node_def)
    return attr_value_pb2.AttrValue(b = True)

  attrs = {
      "_GraphKey":
      key,
      "_Record":
      test_callable
  }
  with ops.get_default_graph()._attr_scope(attrs):
    yield
  finally:
    # if it's a fallback(origin graph), just exit and
    # keep it on the graph
    # else create subgraph and save, then purge all nodes in outer graph.
    if not is_fallback:
      recorded_nodes = ops.get_collection("_test" + key)
      if recorded_nodes and len(recorded_nodes):
        with tf.Graph().as_default():


x = tf.compat.v1.placeholder(tf.float32, shape=(1024, 1024))
y = tf.compat.v1.placeholder(tf.float32, shape=(1024, 1024))

r1 = tf.add(x,y)
r2 = tf.sub(x,y)

r = tf.mul(r1,r2)

print(r)
