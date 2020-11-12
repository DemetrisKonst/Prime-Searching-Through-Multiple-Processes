#include <cmath>

bool prime1 (int n) {
  if (n == 1) return false;

  for (int i = 2; i < n; i++) {
    if (n%i == 0) return false;
  }

  return true;
}

bool prime2 (int n) {
  if (n == 1) return false;

  int limit = (int)(sqrt((float)n));

  for (int i = 2; i < limit; i++) {
    if (n%i == 0) return false;
  }

  return true;
}
