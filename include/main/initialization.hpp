#include <iostream>
#include <cstdlib>
#include <cstring>

int check_arguments (int argc, const char* argv[], int* lower_bound, int* upper_bound, int* children) {
  if (argc != 7) {
    std::cout << "Wrong number of arguments (7 required)" << '\n';
    return 1;
  }

  bool lower_bound_check = false;
  bool upper_bound_check = false;
  bool children_check = false;

  int l;
  int u;
  int w;

  for (int i = 1; i < 7; i+=2) {
    if (strcmp(argv[i], "-l") == 0) {
      if (lower_bound_check) {
        std::cout << "Lower Bound already declared, please check your arguments" << '\n';
        return 2;
      }else{
        l = atoi(argv[i+1]);

        if (l < 0) {
          std::cout << "Lower Bound must be a positive integer" << '\n';
          return 3;
        }

        lower_bound_check = true;
      }
    }else if (strcmp(argv[i], "-u") == 0) {
      if (upper_bound_check) {
        std::cout << "Upper Bound already declared, please check your arguments" << '\n';
        return 2;
      }else{
        u = atoi(argv[i+1]);

        if (u < 0) {
          std::cout << "Upper Bound must be a positive integer" << '\n';
          return 3;
        }

        upper_bound_check = true;
      }
    }else if (strcmp(argv[i], "-w") == 0) {
      if (children_check) {
        std::cout << "Number of children already declared, please check your arguments" << '\n';
        return 2;
      }else{
        w = atoi(argv[i+1]);

        if (w < 0) {
          std::cout << "Number of children must be a positive integer" << '\n';
          return 3;
        }

        children_check = true;
      }
    }else{
      std::cout << "There is no argument named: " << argv[i] << '\n';
      return 4;
    }
  }

  if (! (lower_bound_check && upper_bound_check && children_check)) {
    std::cout << "Arguments missing (check your arguments)" << '\n';
    return 5;
  }

  if (l >= u) {
    std::cout << "Upper bound must be larger than lower bound" << '\n';
    return 6;
  }

  *lower_bound = l;
  *upper_bound = u;
  *children = w;

  return 0;
}
