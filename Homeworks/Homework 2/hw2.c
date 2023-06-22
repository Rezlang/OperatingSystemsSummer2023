#include <ctype.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>
int validMove(int x, int y, int m, int n, int** board) {
  // Check column
  for (int i = 0; i < m; ++i) {
    if (*(*(board + i) + y) == 1) {
      return 0;
    }
  }

  // Check row
  for (int i = 0; i < n; ++i) {
    if (*(*(board + x) + i) == 1) {
      return 0;
    }
  }

  // Check upper-left diagonal
  int row = x;
  int col = y;
  while (row >= 0 && col >= 0) {
    if (*(*(board + row) + col) == 1) {
      return 0;
    }
    row--;
    col--;
  }

  // Check lower-right diagonal
  row = x;
  col = y;
  while (row < m && col < n) {
    if (*(*(board + row) + col) == 1) {
      return 0;
    }
    row++;
    col++;
  }

  // Check upper-right diagonal
  row = x;
  col = y;
  while (row >= 0 && col < n) {
    if (*(*(board + row) + col) == 1) {
      return 0;
    }
    row--;
    col++;
  }

  // Check lower-left diagonal
  row = x;
  col = y;
  while (row < m && col >= 0) {
    if (*(*(board + row) + col) == 1) {
      return 0;
    }
    row++;
    col--;
  }

  return 1;  // Valid move, no attacks detected
}

int solveQueens(int m, int n, int** board, int row, int numQueens,
                int topLevelPid, int* fd, int printLine) {
  if (row == m) {
//<------------------------------Solution-found-------------------------------------->
#ifndef QUIET
    printf("P%d: Found a solution; notifying top-level parent\n", getpid());
#endif
    // for (int i = 0; i < m; i++) {
    //   for (int j = 0; j < n; j++) {
    //     printf("%d ", *(*(board + i) + j));
    //   }
    //   printf("\n");
    // }
    // printf("\n");

    // Period character to be written to the pipeline
    char period = '.';
    int status;

    waitpid(-1, &status, 0);
    // Write a period to the pipeline
    write(*(fd + 1), &period, sizeof(period));
    // Freeing the board
    for (int i = 0; i < m; i++) {
      free(*(board + i));
    }
    free(board);
    free(fd);
    exit(numQueens);
  }
  //<-------------------------------------setup------------------------------------------>

  int queensInRow = 0;
  int* validMoves = calloc(n, sizeof(int));

  // finding valid moves
  for (int col = 0; col < n; col++) {
    if (validMove(row, col, m, n, board)) {
      *(validMoves + queensInRow++) = col;
    }
  }
  // if nothing can be placed
  if (queensInRow == 0) {
#ifndef QUIET
    printf("P%d: Dead end at row #%d\n", getpid(), row);
#endif
    free(validMoves);

    // Freeing the board
    for (int i = 0; i < m; i++) {
      free(*(board + i));
    }
    free(board);
    free(fd);
    exit(numQueens);
  } else {
#ifndef QUIET
    printLine = 1;
    if (queensInRow == 1) {
      printf("P%d: %d possible move at row #%d; creating %d child process...\n",
             getpid(), queensInRow, row, queensInRow);
    } else {
      printf(
          "P%d: %d possible moves at row #%d; creating %d child "
          "processes...\n",
          getpid(), queensInRow, row, queensInRow);
    }
#endif
#ifdef QUIET
    if (printLine == 0) {
      printLine = 1;
      if (queensInRow == 1) {
        printf(
            "P%d: %d possible move at row #%d; creating %d child process...\n",
            getpid(), queensInRow, row, queensInRow);
      } else {
        printf(
            "P%d: %d possible moves at row #%d; creating %d child "
            "processes...\n",
            getpid(), queensInRow, row, queensInRow);
      }
    }
#endif
  }
  // store children's queen counts
  int* numQueensArray = calloc(n, sizeof(int));

  for (int i = 0; i < queensInRow; i++) {
    int col = *(validMoves + i);
    int pid = fork();

    if (pid == 0) {
      //<---------------------------------Child-process---------------------------------->
      // Place the queen in the child's board
      *(*(board + row) + col) = 1;

      // Recursively solve for the next row
      free(numQueensArray);
      free(validMoves);

      solveQueens(m, n, board, row + 1, numQueens + 1, topLevelPid, fd,
                  printLine);

      free(fd);

      exit(numQueens);  // Terminate the child process
    } else if (pid > 0) {
      //<------------------------------Parent-process------------------------------------>

      int status;

#ifdef NO_PARALLEL
      waitpid(pid, &status, 0);
#endif

      waitpid(-1, &status, 0);

      if (WIFEXITED(status)) {
        // If the child process terminated normally, store its return value in
        // numQueensArray
        *(numQueensArray + col) = WEXITSTATUS(status);
      }

    } else {
      perror("fork\n");

      // Freeing the board
      for (int i = 0; i < m; i++) {
        free(*(board + i));
      }
      free(board);
      free(numQueensArray);
      free(validMoves);
      free(fd);
      exit(EXIT_FAILURE);
    }
  }
  free(validMoves);

  int maxQueens = 0;
  for (int i = 0; i < n; i++) {
    if (*(numQueensArray + i) > maxQueens) {
      maxQueens = *(numQueensArray + i);
    }
  }

  free(numQueensArray);

  if (getpid() == topLevelPid) {
    int bufferSize = (m * n) + 1000;
    char* buffer = (char*)malloc(bufferSize * sizeof(char));
    int bufferLength = 0;

    ssize_t bytesRead;
    int status;
    close(*(fd + 1));
    waitpid(-1, &status, 0);
    while ((bytesRead = read(*(fd + 0), buffer + bufferLength,
                             bufferSize - bufferLength)) > 0) {
      bufferLength += bytesRead;
      if (bytesRead == -1) {
        perror("ERROR: Read failure\n");  // Optional: print an error message
        free(buffer);
        // Freeing the board
        for (int i = 0; i < m; i++) {
          free(*(board + i));
        }
        free(board);
        free(fd);
        exit(EXIT_FAILURE);
      }
    }
    close(*(fd + 0));

    *(buffer + bufferLength) = '\0';

    // printf("%s\n", buffer);

    int numSolutions = 0;
    for (int i = 0; i < bufferLength; i++) {
      if (*(buffer + i) == '.') {
        ++numSolutions;
      }
    }
    free(buffer);
    if (m == 8) {
      if (numSolutions > 1) {
        printf("P%d: Search complete; found %d solutions for an %dx%d board\n",
               getpid(), numSolutions, m, n);
      } else if (numSolutions == 1) {
        printf("P%d: Search complete; found %d solution for an %dx%d board\n",
               getpid(), numSolutions, m, n);
      } else if (maxQueens > 1) {
        printf(
            "P%d: Search complete; only able to place %d Queens on an %dx%d "
            "board\n",
            getpid(), maxQueens, m, n);
      } else {
        printf(
            "P%d: Search complete; only able to place %d Queen on an %dx%d "
            "board\n",
            getpid(), maxQueens, m, n);
      }
    } else {
      if (numSolutions > 1) {
        printf("P%d: Search complete; found %d solutions for a %dx%d board\n",
               getpid(), numSolutions, m, n);
      } else if (numSolutions == 1) {
        printf("P%d: Search complete; found %d solution for a %dx%d board\n",
               getpid(), numSolutions, m, n);
      } else if (maxQueens > 1) {
        printf(
            "P%d: Search complete; only able to place %d Queens on a %dx%d "
            "board\n",
            getpid(), maxQueens, m, n);
      } else {
        printf(
            "P%d: Search complete; only able to place %d Queen on a %dx%d "
            "board\n",
            getpid(), maxQueens, m, n);
      }
    }
  }
  free(fd);

  // Freeing the board
  for (int i = 0; i < m; i++) {
    free(*(board + i));
  }
  free(board);
  exit(maxQueens);
}

int main(int argc, char** argv) {
  if (argc != 3) {
    fprintf(stderr, "ERROR: Invalid argument(s)\n");
    fprintf(stderr, "USAGE: %s <m> <n>\n", *argv);
    exit(EXIT_FAILURE);
  }

  int m = atoi(*(argv + 1));  // Convert m argument to an integer
  int n = atoi(*(argv + 2));  // Convert n argument to an integer

  if (m <= 0 || n <= 0) {
    fprintf(stderr, "ERROR: Invalid argument(s)\n");
    fprintf(stderr, "USAGE: %s <m> <n>\n", *argv);
    exit(EXIT_FAILURE);
  }

  if (n < m) {
    int temp = n;
    n = m;
    m = temp;
  }

  int** board = (int**)calloc(m, sizeof(int*));

  if (board == NULL) {
    printf("Memory allocation failed\n");
    exit(EXIT_FAILURE);
  }

  setvbuf(stdout, NULL, _IONBF, 0);

  for (int i = 0; i < m; i++) {
    *(board + i) = (int*)calloc(n, sizeof(int));
    if (*(board + i) == NULL) {
      printf("Memory allocation failed\n");

      // Freeing the board
      for (int i = 0; i < m; i++) {
        if (*(board + i) != NULL) {
          free(*(board + i));
        }
      }
      free(board);
      exit(EXIT_FAILURE);
    }
  }

  int numQueens = 0;

  int topLevelPid = getpid();
  int* fd = (int*)calloc(2, sizeof(int));
  pipe(fd);  // Create a pipe for communication between parent and child process

  if (getpid() == topLevelPid && m == 8) {
    printf("P%d: Solving the (m,n)-Queens problem for an %dx%d board\n",
           getpid(), m, n);
  } else if (getpid() == topLevelPid) {
    printf("P%d: Solving the (m,n)-Queens problem for a %dx%d board\n",
           getpid(), m, n);
  }

  int printLine = 0;

  solveQueens(m, n, board, 0, numQueens, topLevelPid, fd,
              printLine);  // Start solving from the first row

  free(fd);

  // Freeing the board
  for (int i = 0; i < m; i++) {
    free(*(board + i));
  }
  free(board);

  return 0;
}
