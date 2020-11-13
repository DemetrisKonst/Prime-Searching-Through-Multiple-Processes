#pragma once

#include <iostream>
#include <cstring>
#include <cstdlib>

struct PrimeItem {
  uint32_t number;
  double time;

  PrimeItem () {
    number = 0;
    time = 0.0;
  }

  PrimeItem (int n, double t) : number(n), time(t) {}

  void print () {
    std::cout << "Number: " << number << '\n';
    // std::cout << "Time: " << time << '\n';
  }
};

struct PipeMessage {
  int type;
  void* data;
};
