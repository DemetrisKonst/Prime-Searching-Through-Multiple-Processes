#pragma once

#include <iostream>
#include <cstring>
#include <cstdlib>

struct PrimeItem {
  int number;
  double time;

  PrimeItem () {
    number = 0;
    time = 0.0;
  }

  PrimeItem (int n, double t) : number(n), time(t) {}

  void operator= (const PrimeItem &item) {
    number = item.number;
    time = item.time;
  }

  void print () {
    std::cout << number << " " << time << " ";
    // std::cout << "Number: " << number << '\n';
    // std::cout << "Time: " << time << '\n';
  }
};
