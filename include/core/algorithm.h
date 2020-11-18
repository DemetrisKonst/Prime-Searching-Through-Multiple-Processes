#pragma once

#include <cmath>

bool prime1 (long int n) {
  if (n == 1) return false;
  if (n == 2 || n == 3) return true;

  for (long int i = 2; i < n; i++) {
    if (n%i == 0) return false;
  }

  return true;
}

bool prime2 (long int n) {
  if (n == 1) return false;
  if (n == 2 || n == 3) return true;

  int limit = (int)(sqrt((double)n));

  for (long int i = 2; i <= limit; i++) {
    if (n%i == 0) return false;
  }

  return true;
}

bool prime3 (long int n) {
  if (n == 1) return false;
  if (n == 2 || n == 3) return true;

  if (n%2 == 0 || n%3 == 0) return(false);

  for (long int i = 5 ; i*i <= n ; i+=6) {
    if (n%i == 0 || n%(i+2) == 0) return(false);
  }

  return(true);
}
