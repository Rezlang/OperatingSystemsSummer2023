#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int hash(char* string, int cacheSize) {
  int hash = 0;
  int i = 0;
  while (*(string + i) != '\0') {
    hash += *(string + i);
    ++i;
  }
  return hash % cacheSize;
}

bool isWord(const char* word) {
  if (strlen(word) < 2) return false;
  for (int i = 0; *(word + i) != '\0'; i++) {
    if (!isalpha(*(word + i))) {
      return false;
    }
  }
  return true;
}

// Word "up" ==> 8 [realloc]
// Word "and" ==> 1 [calloc]
// Word "down" ==> 15 [nop]
int addWord(char** cache, char* word, int cacheSize) {
  char* allocType = "nop";
  if (!isWord(word)) return 0;
  int index = hash(word, cacheSize);
  if (*(cache + index) == NULL) {
    *(cache + index) = (char*)calloc(strlen(word) + 1, sizeof(char));
    allocType = "calloc";
  } else if (sizeof(*(cache + index)) != strlen(word) + 1) {
    *(cache + index) = (char*)realloc(*(cache + index), strlen(word) + 1);
    allocType = "realloc";
  }
  strcpy(*(cache + index), word);
  printf("Word \"%s\" ==> %d [%s]\n", word, index, allocType);
  return 1;
}

int readInFile(char** cache, char* fileName, int cacheSize) {
  FILE* file = fopen(fileName, "r");
  if (file == NULL) {
    printf("stderr, Failed to open the file.\n");
    return 0;
  }

  char word[128];
  char* wordPtr = word;
  int c;

  while ((c = fgetc(file)) != EOF) {
    if (isalpha(c)) {
      wordPtr = word;
      *wordPtr++ = c;
      while ((c = fgetc(file)) != EOF && isalpha(c) &&
             wordPtr < word + sizeof(word) - 1) {
        *wordPtr++ = c;
      }
      *wordPtr = '\0';
      int allocType = addWord(cache, word, cacheSize);
    }
  }

  fclose(file);
  return 1;
}

int main(int argc, char** argv) {
  if (argc != 3) {
    fprintf(stderr, "Incorrect arguments provided\n");
  }
  int cacheSize = atoi(*(argv + 1));
  char* fileName = *(argv + 2);
  char** cache = (char**)calloc(cacheSize, sizeof(char*));
  readInFile(cache, fileName, cacheSize);
}
