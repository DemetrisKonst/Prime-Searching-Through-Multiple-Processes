#pragma once

#include <iostream>

struct PrimeItem {
  long int number;
  double time;

  PrimeItem () {
    number = 0;
    time = 0.0;
  }

  PrimeItem (long int n, double t) : number(n), time(t) {}

  void operator= (const PrimeItem &item) {
    number = item.number;
    time = item.time;
  }

  void print () {
    std::cout << number << " " << time << " ";
  }
};
