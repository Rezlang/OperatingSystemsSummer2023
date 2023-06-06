#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
int main(int argc, char** argv) {
  if (argc != 3) {
    fprintf(stderr, "Incorrect arguments provided\n");
  }
  int cacheSize = atoi(*(argv + 1));
  char* fileName = *(argv + 2);
  char** cache = (char**)calloc(cacheSize, sizeof(char*));
}

int hash(char* string) { return 0; }

int readIn(char** cache, char* fileName) {}

bool isWord(const char* word) {
  if (length(word) < 2) return false;
  for (int i = 0; *(word + i) != '\0'; i++) {
    if (!isalpha(*(word + i))) {
      return false;
    }
  }
  return true;
}

int addWord(char** cache, char* word) {
  if (!isWord(word)) return 0;
  int index = hash(word);
  *(cache + index) = (char*)calloc(length(word), sizeof(char));
  strcpy(*(cache + index), word);
  return 1;
}