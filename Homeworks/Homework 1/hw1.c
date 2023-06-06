#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Takes input of a string and adds the ascii values of the characters
// returns that value modulo the size of the cache.
int hash(char* string, int cacheSize) {
  int hash = 0;
  int i = 0;
  while (*(string + i) != '\0') {
    hash += *(string + i);
    ++i;
  }
  return hash % cacheSize;
}

// Determines whether a string is a valid word
bool isWord(const char* word) {
  if (strlen(word) < 2) return false;
  for (int i = 0; *(word + i) != '\0'; i++) {
    if (!isalpha(*(word + i))) {
      return false;
    }
  }
  return true;
}

// Adds a word to the correct place in the cache
int addWord(char** cache, char* word, int cacheSize) {
  // default is index is the correct size
  char* allocType = "nop";
  if (!isWord(word)) return 0;
  int index = hash(word, cacheSize);
  if (*(cache + index) == NULL) {
    // if index is empty
    *(cache + index) = (char*)calloc(strlen(word) + 1, sizeof(char));
    allocType = "calloc";
  } else if (strlen(*(cache + index)) != strlen(word)) {
    // if index is the wrong size
    *(cache + index) = (char*)realloc(*(cache + index), strlen(word) + 2);
    allocType = "realloc";
  }
  strcpy(*(cache + index), word);
  printf("Word \"%s\" ==> %d %c%s%c\n", word, index, 91, allocType, 93);
  return 1;
}

// takes file input, then reads and processes each word
int readInFile(char** cache, char* fileName, int cacheSize) {
  FILE* file = fopen(fileName, "r");
  if (file == NULL) {
    fprintf(stderr, "ERROR: Failed to open the file.\n");
    exit(3);
  }

  char* word = (char*)calloc(128, sizeof(char));
  char* wordPtr;
  int c;

  while ((c = fgetc(file)) != EOF) {
    if (isalpha(c)) {
      wordPtr = word;
      *wordPtr++ = c;
      while ((c = fgetc(file)) != EOF && isalpha(c) &&
             wordPtr < word + 128 - 1) {
        *wordPtr++ = c;
      }
      *wordPtr = '\0';

      addWord(cache, word, cacheSize);

      // resetting wordPtr to start of word for the next word
      wordPtr = word;
    }
  }

  fclose(file);
  free(word);
  return 0;
}

int main(int argc, char** argv) {
  // Error checking
  if (argc != 3) {
    fprintf(stderr, "ERROR: Incorrect arguments provided\n");
    return 1;
  }
  int cacheSize = atoi(*(argv + 1));
  if (cacheSize == 0 && *(*(argv + 1)) != '0') {
    fprintf(stderr, "ERROR: Invalid size\n");
    return 2;
  }
  // File reading
  char* fileName = *(argv + 2);
  char** cache = (char**)calloc(cacheSize, sizeof(char*));
  readInFile(cache, fileName, cacheSize);

  // Final printing of values and freeing of memory
  printf("\nCache:\n");
  for (int i = 0; i < cacheSize; ++i) {
    if (*(cache + i) == NULL) continue;
    printf("index %d ==> \"%s\"\n", i, *(cache + i));
    free(*(cache + i));
  }
  free(cache);
  return 0;
}
