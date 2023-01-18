#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

#define LENGTH 512

int main() {
  int fd = open("pg2243.txt", O_RDONLY);
  if(fd == -1) {
    printf("Could not find file. Exiting\n");
    exit(1);
  }

  char buffer[LENGTH] = {0};
  read(fd, buffer, LENGTH);
  buffer[LENGTH] = '\0';
  printf("First %d bytes of the file:\n", LENGTH-1);
  printf("  %s\n", buffer);
  close(fd);
  return 0;
}
