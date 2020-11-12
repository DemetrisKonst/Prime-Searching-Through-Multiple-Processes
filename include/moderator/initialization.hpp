#include <iostream>
#include <cstdlib>
#include <cstring>

int check_arguments (int argc, const char* argv[], int* lower_bound, int* upper_bound, int* children, int* number, int* write_fd) {
  if (argc != 6) {
    std::cout << "Wrong number of arguments (5 required)" << '\n';
    return 1;
  }

  int l = atoi(argv[1]);
  int u = atoi(argv[2]);
  int w = atoi(argv[3]);
  int n = atoi(argv[4]);
  int f = atoi(argv[5]);


  *lower_bound = l;
  *upper_bound = u;
  *children = w;
  *number = n;
  *write_fd = f;

  return 0;
}
