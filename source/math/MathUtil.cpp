#include "MathUtil.h"

// this is Daniel J. Bernstein's hash algorithm
// requires c string as input
unsigned long hash(const char* str) {
  unsigned long hash = 5381;
  int c;

  while ((c = *str++))
    hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

  return hash;
}
