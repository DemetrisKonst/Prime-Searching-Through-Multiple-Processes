#pragma once

#include <iostream>
#include <cstring>
#include <cstdlib>

struct PrimeItem {
  int number;
  double cpu_time;
  double real_time;

  PrimeItem (char* buffer) {
    const char col[2] = ":";
    char* no_identifier = strtok(buffer, col);
    no_identifier = strtok(NULL, col);

    const char dash[2] = "-";
    char* number_string = strtok(no_identifier, dash);
    char* times_string = strtok(NULL, dash);

    const char comma[2] = ",";
    char* cpu_time_string = strtok(times_string, comma);
    char* real_time_string = strtok(NULL, comma);

    number = atoi(number_string);
    cpu_time = atof(cpu_time_string);
    real_time = atof(real_time_string);
  }

  void print () {
    std::cout << "Number: " << number << '\n';
    // std::cout << "CPU time: " << cpu_time << '\n';
    // std::cout << "Real time: " << real_time << '\n';
  }
};
