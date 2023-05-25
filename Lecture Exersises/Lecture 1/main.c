#include "reverse.h"

int main() {
  char str[] = "Hello, world!";
  printf("Before reversal: %s\n", str);
  char *reversed = reverse(str);
  if (reversed != NULL) {
    printf("After reversal: %s\n", reversed);
  } else {
    printf("Failed to reverse the string!\n");
  }
  return 0;
}