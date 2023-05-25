#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *reverse(char *s) {
  int len = strlen(s);
  char *buffer = (char *)malloc((len + 1) * sizeof(char));
  if (buffer == NULL) {
    printf("Memory allocation failed!\n");
    return NULL;
  }

  int i;
  for (i = 0; i < len; i++) *(buffer + i) = *(s + len - i - 1);

  *(buffer + len) = '\0';  // Null terminate the string

  for (i = 0; i <= len; i++) *(s + i) = *(buffer + i);

  free(buffer);  // Free the dynamically allocated memory

  return s;
}
