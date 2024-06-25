class Test(object):
  @classmethod
  def test(cls):
    print("a")

class Test2(Test):
  @classmethod
  def test(cls):
    print("b")

class Test3(Test2):
  def self_m(self):
    print("v")


Test.test()

Test2.test()

Test3.test()

a=Test()
a.test()
b=Test3()
b.test()
