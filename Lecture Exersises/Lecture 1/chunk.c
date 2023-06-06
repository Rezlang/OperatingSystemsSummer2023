#include <fcntl.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int chunk(int num, char* filename) {
  int file = open(filename, O_RDONLY);
  if (file == -1) {
    perror("Could not open file");
    return 1;
  }

  char* buffer = (char*)malloc((num + 1) * sizeof(char));

  int flag = 0;
  int readCount = -1;
  while ((readCount = read(file, buffer, num)) > 0) {
    if (flag == 0) {
      flag = 1;
    } else
      printf("|");

    off_t offset = lseek(file, num, SEEK_CUR);
    if (offset == (off_t)-1) {
      perror("could not skip");
      return 2;
    }
    if (readCount == 0) break;
    *(buffer + readCount) = '\0';
    printf("%s", buffer);
  }
  printf("\n");
  close(file);
  return 0;
}

int main(int argc, char** argv) {
  // Check if there are enough command line arguments
  if (argc < 3) {
    printf("Two arguments are needed: a file name and an integer\n");
    return 1;
  }
  char* filename = *(argv + 2);
  int num = atoi(*(argv + 1));

  return chunk(num, filename);
}
