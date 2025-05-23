// REQUIRES: continuous-mode

// RUN: rm -rf %t.dir
// RUN: %clangxx_pgogen=%t.dir -fprofile-continuous -lpthread %s -o %t.exe -mllvm -disable-vp -fprofile-update=atomic
// RUN: %run %t.exe
// RUN: llvm-profdata show --counts --function=accum  %t.dir/default_*.profraw | FileCheck %s
// CHECK:    Block counts: [100000, 4]

#include <thread>

int x = 0;
void accum(int n) {
  for (int i = 0; i < n; i++)
    x += i; // don't care about accuracy, no need for atomic.
}

int main() {
  int init_value = 10000;
  auto t1 = std::thread(accum, 1*init_value);
  auto t2 = std::thread(accum, 2*init_value);
  auto t3 = std::thread(accum, 3*init_value);
  auto t4 = std::thread(accum, 4*init_value);

  t1.join();
  t2.join();
  t3.join();
  t4.join();
  return !x;
}
