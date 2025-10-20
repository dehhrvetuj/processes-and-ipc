#include <stdio.h>    // puts(), printf(), perror(), getchar()
#include <stdlib.h>   // exit(), EXIT_SUCCESS, EXIT_FAILURE
#include <unistd.h>   // getpid(), getppid(), fork(), pipe(), dup2(), execlp(), close()
#include <sys/wait.h> // wait()

#define READ  0       // pipe read end
#define WRITE 1       // pipe write end

void child_a(int fd[]) 
{
    close(fd[READ]);                          // not used in child A
    
    if (dup2(fd[WRITE], 1) == -1)             // redirect stdout → pipe write end
    {           
        perror("dup2 in child A fails");
        exit(EXIT_FAILURE);
    }
    
    close(fd[WRITE]);                         // close original descriptor
    
    execlp("ls", "ls", "-F", "-1", NULL);     // execute "ls -F -1"
    perror("execlp fails in A");
    exit(EXIT_FAILURE);
}

void child_b(int fd[])
{
    close(fd[WRITE]);                         // not used in child B
    
    if (dup2(fd[READ], 0) == -1)              // redirect stdin ← pipe read end
    {           
        perror("dup2 in child B fails");
        exit(EXIT_FAILURE);
    }
    
    close(fd[READ]);                          // close original descriptor
    
    execlp("nl", "nl", NULL);                 // execute "nl"
    perror("execlp fails in B");
    exit(EXIT_FAILURE);
}

void parent(pid_t pid)
{
    // Parent can report creation if needed
}

void close_fd(int fd[])
{
    close(fd[READ]);
    close(fd[WRITE]);
}

int main(void) 
{
    int fd[2];              // pipe descriptors
    pid_t pid_a, pid_b;     // child PIDs

    if (pipe(fd)) 
    {
        perror("pipe failed");
        exit(EXIT_FAILURE);
    }

    // Fork child A
    switch (pid_a = fork()) 
    {
        case -1:
            perror("fork child A failed");
            close_fd(fd);
            exit(EXIT_FAILURE);
            break;
        case 0:
            child_a(fd);
            break;
        default:
            parent(pid_a);
            break;
    }

    // Fork child B
    switch (pid_b = fork()) 
    {
        case -1:
            perror("fork child B failed");
            close_fd(fd);
            exit(EXIT_FAILURE);
            break;
        case 0:
            child_b(fd);
            break;
        default:
            parent(pid_b);
            break;
    }

    // Parent closes both ends
    close_fd(fd);

    pid_t pid;
    int status;

    // Wait for first child
    pid = wait(&status);
    if (WIFEXITED(status)) 
    {
        // printf("Child %ld exited with status %d\n", (long)pid, WEXITSTATUS(status));
    }

    // Wait for second child
    pid = wait(&status);
    if (WIFEXITED(status)) 
    {
        // printf("Child %ld exited with status %d\n", (long)pid, WEXITSTATUS(status));
    }

    return 0;
}
