#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>

int main() {
  int pipe_fds[2] = {0};
  int succ = pipe(pipe_fds);

  if(succ == -1) {
    printf("Couldn't create pipe!\n");
    exit(1);
  }
  int in_p = pipe_fds[1];
  int out_p = pipe_fds[0];

  pid_t child_pid = fork();
  if(child_pid == 0) {
    // Child Runs this
    close(in_p);
    dup2(out_p, STDIN_FILENO);
    execl("/usr/bin/sort", "sort", NULL);
    exit(1);
  }
  else {
    // Parent Runs this
    close(out_p);
    dup2(in_p, STDOUT_FILENO);
    printf("Hello\n");
    printf("World\n");
    printf("My\n");
    printf("Name\n");
    printf("Is\n");
    printf("Happy\n");
    printf("Apple\n");
    close(in_p);
  }
  return 0;
}