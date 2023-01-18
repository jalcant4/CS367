#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

int main() {
  pid_t child_pid = fork();
  if(child_pid == 0) {
    // Child Runs this
    int fd_wc = open("results.txt", O_WRONLY | O_TRUNC | O_CREAT, 0644);
    dup2(fd_wc, STDOUT_FILENO);
    execl("/usr/bin/wc", "wc", "-l", "pg2243.txt", NULL);
    exit(1);
  }
  else {
    wait(NULL);
    printf("Parent quitting\n");
  }
  return 0;
}
