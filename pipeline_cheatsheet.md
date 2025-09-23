# Process Pipelines & Pipes — Cheat Sheet (中英文对照)

---

## Part I — General Questions (一般性问题)

### 1) What do we mean with a process pipeline?  
**EN:** A pipeline is a sequence of processes connected so that the output of one becomes the input of the next (e.g., `A | B | C`).  
**中文：** 管道（pipeline）是一系列相连的进程，前一个进程的输出通过管道成为下一个进程的输入（如 `A | B | C`）。

---

### 2) What is a pipe?  
**EN:** A pipe is a unidirectional kernel buffer with two file descriptors: one for reading and one for writing.  
**中文：** 管道是由内核实现的单向缓冲区，包含两个文件描述符：一个读端，一个写端。

---

### 3) How are pipes used to construct process pipelines?  
**EN:** Use `pipe()` to create file descriptors, then `fork()` children.  
- Producer redirects `stdout` to the pipe’s write end (`dup2(fd[WRITE], STDOUT_FILENO)`).  
- Consumer redirects `stdin` to the pipe’s read end (`dup2(fd[READ], STDIN_FILENO)`).  
Then both call `exec()` to run commands.  
**中文：** 调用 `pipe()` 创建文件描述符，然后 `fork()` 出子进程。  
- 生产者用 `dup2(fd[WRITE], STDOUT_FILENO)` 把标准输出重定向到管道写端。  
- 消费者用 `dup2(fd[READ], STDIN_FILENO)` 把标准输入重定向到管道读端。  
最后两个子进程调用 `exec()` 运行命令。

---

### 4) Show an example of a process pipeline in the terminal (shell).  
**EN:**  
```bash
ls -F -1 | nl
```  
`ls` outputs file names, piped into `nl` which numbers the lines.  
**中文：**  
```bash
ls -F -1 | nl
```  
`ls` 输出文件名，通过管道传递给 `nl`，由 `nl` 给每一行加上行号。

---

### 5) How are shell (terminal) commands implemented?  
**EN:** The shell parses the command line, creates pipes if needed, forks children, sets up redirections with `dup2`, closes unused fds, calls `exec` to run commands, and `wait`s for them to finish.  
**中文：** Shell 解析命令行，必要时创建管道，`fork` 子进程，用 `dup2` 设置输入输出重定向，关闭不需要的文件描述符，然后调用 `exec` 运行命令，最后 `wait` 等待子进程结束。

---

### 6) What happens to the file descriptor table after a successful creation of a new pipe?  
**EN:** Two new file descriptors are added: `fd[0]` (read end) and `fd[1]` (write end). After `fork()`, they are inherited by child processes.  
**中文：** 会新增两个文件描述符：`fd[0]`（读端）和 `fd[1]`（写端）。调用 `fork()` 后，这两个描述符会被子进程继承。

---

## Part II — Program-Specific Questions (与程序相关的问题)

### 7) How many times does the parent call `fork` and why?  
**EN:** Twice: once to create the producer (`ls`), once to create the consumer (`nl`). Each stage runs in its own process.  
**中文：** 父进程调用两次 `fork()`：一次创建生产者（`ls`），一次创建消费者（`nl`）。管道的每个阶段都运行在独立的进程中。

---

### 8) Why do the children need to call `execlp()`?  
**EN:** To replace the child’s code with the external program while preserving the redirections already set up.  
**中文：** 用外部程序替换子进程自身的代码，同时保留已经设置好的输入输出重定向。

---

### 9) Explain how each child is able to redirect `stdin` or `stdout` from or to the pipe.  
**EN:**  
- Producer: `dup2(fd[WRITE], STDOUT_FILENO)` → standard output goes into the pipe.  
- Consumer: `dup2(fd[READ], STDIN_FILENO)` → standard input comes from the pipe.  
**中文：**  
- 生产者：`dup2(fd[WRITE], STDOUT_FILENO)` → 标准输出写入管道。  
- 消费者：`dup2(fd[READ], STDIN_FILENO)` → 标准输入来自管道。

---

### 10) How will the consumer know when there is no more data to expect from the pipe?  
**EN:** When all write ends of the pipe are closed (by the producer and the parent), `read` on the consumer side returns 0 (EOF).  
**中文：** 当管道的所有写端（包括生产者和父进程的写端）都关闭时，消费者的 `read` 返回 0（EOF），表示没有更多数据。

---

### 11) Why is it important for a process to close any pipe file descriptors it does not intend to use?  
**EN:**  
- Prevents file descriptor leaks.  
- Ensures EOF is properly delivered (extra open write ends keep the reader blocked).  
- Avoids accidental reads or writes on the wrong end.  
**中文：**  
- 防止文件描述符泄漏。  
- 确保 EOF 能正确传递（如果写端未关闭，读端可能一直阻塞）。  
- 避免错误的输入输出。

---

### 12) What could happen if you close a read descriptor too early?  
**EN:** The process will lose access to incoming data; it may get immediate EOF or errors, breaking the pipeline logic.  
**中文：** 如果过早关闭读端，进程将无法继续读取数据，可能立即遇到 EOF 或报错，导致管道逻辑中断。

---
