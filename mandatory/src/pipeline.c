#include <stdio.h>    // puts(), printf(), perror(), getchar()
#include <stdlib.h>   // exit(), EXIT_SUCCESS, EXIT_FAILURE
#include <unistd.h>   // getpid(), getppid(), fork(), pipe(), dup2(), execlp(), close()
#include <sys/wait.h> // wait()

#define READ  0       // index for pipe read end
#define WRITE 1       // index for pipe write end

void child_a(int fd[]) {
  // Child A: producer process (runs `ls -F -1` and writes output into the pipe)
  printf(" CHILD <%ld> I'm alive and my PPID = %ld.\n",
         (long) getpid(), (long) getppid());
  
  close(fd[READ]);             // Child A does not need the read end of the pipe
  
  dup2(fd[WRITE], 1);          // Redirect STDOUT (fd=1) to the pipe's write end
  
  close(fd[WRITE]);            // Close the original write descriptor after dup2
  
  execlp("ls", "ls", "-F", "-1", NULL); // Replace child process with "ls -F -1"
  
  perror("execlp");            // Only executed if execlp() fails
  exit(EXIT_FAILURE);          // Exit with failure if exec did not succeed
}

void child_b(int fd[]) {
  // Child B: consumer process (runs `nl` and reads input from the pipe)
  printf(" CHILD <%ld> I'm alive and my PPID = %ld.\n",
         (long) getpid(), (long) getppid());

  close(fd[WRITE]);            // Child B does not need the write end of the pipe
  
  dup2(fd[READ], 0);           // Redirect STDIN (fd=0) to the pipe's read end
  
  close(fd[READ]);             // Close the original read descriptor after dup2
  
  execlp("nl", "nl", NULL);    // Replace child process with "nl"
  
  perror("execlp");            // Only executed if execlp() fails
  exit(EXIT_FAILURE);          // Exit with failure if exec did not succeed
}

void parent(pid_t pid) {
  // Parent: just reports that it has created a child process
  printf("PARENT <%ld> Spawned a child with PID = %ld.\n",
         (long) getpid(), (long) pid);
}

int main(void) {
  int fd[2];                   // pipe descriptors: fd[0] = read end, fd[1] = write end
  pid_t pid_a, pid_b;          // child PIDs
  
  pipe(fd);                    // Create a pipe for communication

  // First fork: create child A
  switch (pid_a = fork()) {
  case -1:                     // fork() error handling
    perror("fork failed");
    close(fd[READ]);
    close(fd[WRITE]);
    exit(EXIT_FAILURE);
  case 0:                      // Child process branch
    child_a(fd);               // Run child A code
  default:                     // Parent process branch
    parent(pid_a);             // Print parent message
  }
  
  // Second fork: create child B
  switch (pid_b = fork()) {
  case -1:                     // fork() error handling
    perror("fork failed");
    close(fd[READ]);
    close(fd[WRITE]);
    exit(EXIT_FAILURE);
  case 0:                      // Child process branch
    child_b(fd);               // Run child B code
  default:                     // Parent process branch
    parent(pid_b);             // Print parent message
  }
  
  // Parent process: not part of the pipeline, close both ends
  close(fd[READ]);
  close(fd[WRITE]);
  
  pid_t pid;
  int status;
  
  // First wait(): reap whichever child exits first
  pid = wait(&status);
  if (WIFEXITED(status)) {
    printf("PARENT <%ld> Child with PID = %ld and exit status = %d terminated.\n",
           (long) getpid(), (long) pid, WEXITSTATUS(status));
  }
  
  // Second wait(): reap the remaining child
  pid = wait(&status);
  if (WIFEXITED(status)) {
    printf("PARENT <%ld> Child with PID = %ld and exit status = %d terminated.\n",
           (long) getpid(), (long) pid, WEXITSTATUS(status));
  }

  return 0;
}
