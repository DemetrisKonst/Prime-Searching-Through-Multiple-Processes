#include <iostream>

#include "../include/moderator/initialization.hpp"

int main(int argc, char const *argv[]) {
  int lower_bound;
  int upper_bound;
  int children;
  int number;

  int ca = check_arguments(argc, argv, &lower_bound, &upper_bound, &children, &number);
  if (ca != 0) {
    exit(ca);
  }
  std::cout << number << ": Numbers = " << upper_bound - lower_bound + 1 << '\n';

  std::cout << number << ": Lower Bound = " << lower_bound << '\n';
  std::cout << number << ": Upper Bound = " << upper_bound << '\n';
  // std::cout << number << ": Children = " << children << '\n';

  exit(EXIT_SUCCESS);
}
