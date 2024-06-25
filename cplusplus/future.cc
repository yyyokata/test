/*************************************************************************
 * Copyright (C) [2020-2023] by Cambricon, Inc.
 * Description: 
 *************************************************************************/
#include <future>
#include <iostream>
#include <thread>

void Test(std::future<bool> test, std::future<bool> test2) {
  while (test.wait_for(std::chrono::milliseconds(1)) == std::future_status::timeout) {}
  std::cout << "work";
  while (test2.wait_for(std::chrono::milliseconds(1)) == std::future_status::timeout) {
    std::cout << "work too";
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
  }
}

int main() {
  std::promise<bool> start;
  std::promise<bool> end;
  std::thread test(Test, start.get_future(), end.get_future());
  start.set_value(true);
  std::this_thread::sleep_for(std::chrono::milliseconds(10000));
  end.set_value(true);
  test.join();
  return 0;
}
