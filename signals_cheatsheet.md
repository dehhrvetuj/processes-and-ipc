# Signals Cheat Sheet – 中英文对照

---

## 1. What is a signal? / 什么是信号？
**EN:** A signal is an asynchronous software interrupt delivered by the kernel to a process to notify it of an event (e.g., `SIGINT` on Ctrl-C, `SIGSEGV` on invalid memory access).  
**ZH:** 信号是由内核发送给进程的 **异步软件中断**，用来通知某个事件（例如 Ctrl-C 产生 `SIGINT`，非法内存访问产生 `SIGSEGV`）。

---

## 2. How do signals relate to exceptions and interrupts? / 信号与异常和中断的关系？
**EN:**  
- Hardware **exceptions** (like divide-by-zero, page fault) → kernel → deliver **synchronous signals** (e.g., `SIGFPE`, `SIGSEGV`).  
- Hardware **interrupts** (like keyboard, timer) → kernel handles → may send **asynchronous signals** (e.g., `SIGINT`).  
**ZH:**  
- 硬件 **异常**（如除零、缺页）会触发内核，然后内核向出错进程发送 **同步信号**。  
- 硬件 **中断**（如键盘、定时器）由内核处理，然后可能向进程发送 **异步信号**。

---

## 3. What is a signal handler? / 什么是信号处理函数？
**EN:** A signal handler is a user-defined function that the kernel invokes when a specific signal is delivered.  
**ZH:** 信号处理函数是用户定义的函数，内核在某个信号到达时会自动调用它。

---

## 4. How do you register a signal handler? / 如何注册信号处理函数？
**EN:**  
```c
signal(SIGINT, handler);      // simple
sigaction(SIGINT, &sa, NULL); // robust
```  
**ZH:** 可以用 `signal()` 注册（简单但移植性差），或用 `sigaction()` 注册（推荐，功能更强）。

---

## 5. What happens if you don’t register a signal handler? / 如果不注册信号处理函数会怎样？
**EN:** The **default action** occurs: e.g., `SIGSEGV` → terminate (core dump), `SIGINT` → terminate, `SIGCHLD` → ignored.  
**ZH:** 会执行该信号的 **默认动作**：例如 `SIGSEGV` → 终止并产生 core dump；`SIGINT` → 终止；`SIGCHLD` → 默认忽略。

---

## 6. What causes a segfault? / 什么会导致段错误？
**EN:** Accessing invalid memory: dereferencing `NULL`, out-of-bounds, use-after-free, writing read-only memory.  
**ZH:** 访问非法内存会产生段错误：如解引用 `NULL`，数组越界，释放后继续使用，写只读内存等。

---

## 7. What is meant by a synchronous signal? / 什么是同步信号？
**EN:** A signal caused directly by the current instruction, e.g., `SIGFPE` (divide-by-zero), `SIGSEGV`.  
**ZH:** 同步信号是由当前指令直接导致的信号，例如 `SIGFPE`（除零）、`SIGSEGV`（非法内存访问）。

---

## 8. What does the system call `pause()` do? / 系统调用 `pause()` 做什么？
**EN:** Suspends the process until any signal is caught and its handler returns. Then `pause()` returns `-1` with `errno = EINTR`.  
**ZH:** `pause()` 会让进程挂起，直到捕获到信号并返回处理函数为止。返回时返回值为 `-1`，并设置 `errno = EINTR`。

---

## 9. What happens when you press Ctrl-C in the controlling terminal of a process? / 当你在控制终端按 Ctrl-C 会发生什么？
**EN:** The terminal driver sends `SIGINT` to the foreground process group. The default action is termination. If a handler is set, it runs instead.  
**ZH:** 终端驱动程序会向前台进程组发送 `SIGINT`。默认动作为终止。如果用户注册了处理函数，就会执行处理函数。

---

## 10. How do you send signals to other processes? / 如何向其他进程发送信号？
**EN:**  
- From shell: `kill -USR1 <pid>`  
- From code: `kill(pid, SIGUSR1)` or `raise(SIGUSR1)` (self).  
**ZH:**  
- 在 shell 中：`kill -USR1 <pid>`  
- 在代码中：`kill(pid, SIGUSR1)` 或 `raise(SIGUSR1)`（发给自己）。

---

## 11. Why is the keyword `volatile` needed for `done`? / 为什么 `done` 要用 `volatile`？
**EN:** Without `volatile`, the compiler may cache `done` in a register, missing updates from the handler. `volatile` forces memory access every time.  
**ZH:** 如果没有 `volatile`，编译器可能会把 `done` 缓存在寄存器里，导致主程序看不到处理函数的修改。`volatile` 强制每次读写都访问内存。

---

## 12. Why is the datatype `sig_atomic_t` needed for `done`? / 为什么 `done` 要用 `sig_atomic_t`？
**EN:** `sig_atomic_t` guarantees atomic reads/writes relative to signal handling, preventing partial updates.  
**ZH:** `sig_atomic_t` 保证对变量的读写是 **原子操作**（相对于信号处理而言），避免“读到一半”的情况。

---

## 13. Why is it more efficient to use `pause()` instead of looping on `done`? / 为什么用 `pause()` 比循环检查 `done` 更高效？
**EN:** Busy-waiting wastes CPU. `pause()` blocks until a signal arrives, saving CPU and reacting immediately.  
**ZH:** 忙等循环会浪费 CPU 资源。`pause()` 会阻塞进程，直到信号到达才唤醒，既节省 CPU，又能及时响应。

---
