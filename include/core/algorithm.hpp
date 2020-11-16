#pragma once

#include <cmath>
#include <cstdio>

bool prime1 (long int n) {
  if (n == 1) return false;

  for (long int i = 2; i < n; i++) {
    if (n%i == 0) return false;
  }

  return true;
}

bool prime2 (long int n) {
  if (n == 1) return false;

  int limit = (int)(sqrt((double)n));

  for (long int i = 2; i <= limit; i++) {
    if (n%i == 0) return false;
  }

  return true;
}

bool prime3 (long int n) {
  if (n == 1) return false;
  if (n == 2) return true;

  if (n%2 == 0) return false;

  int limit = (int)(sqrt((double)n));

  for (long int i = 3; i <= limit; i+=2) {
    if (n%i == 0) return false;
  }

  return true;
}
