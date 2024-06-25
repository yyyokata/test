from functools import wraps
import contextlib

@contextlib.contextmanager
def test_scope(op):
  try:
    yield
  finally:
    print(1)

def decorator(gpu_opt, op):
  def dec(func):
    @wraps(func)
    def recorder(*args, **kwargs):
      print("e")
      with test_scope(op):
        func(*args, **kwargs)
    return recorder
  return dec


def test_decorator_factory(gpu_opt, op):
  # if gpu
  def normal_decorator(build_graph):
    @wraps(build_graph)
    def normal(*args, **kwargs):
      print("d")
      build_graph(*args, **kwargs)
    return normal

  def empty_decorator(build_graph):
    @wraps(build_graph)
    def empty(*args, **kwargs):
      print("f")
    return empty

  if gpu_opt:
    return empty_decorator
  return decorator(gpu_opt, op)

print("begin1")

@test_decorator_factory(True, "a")
def test_func():
  print(8)

print("begin2")
@test_decorator_factory(False, "a")
def test_func2():
  print(9)

test_func()
test_func2()
