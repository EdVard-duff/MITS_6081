# 虚拟化

## 虚拟 CPU 

### 
操作系统将一个运行中的程序抽象为"进程", 并通过时分共享给进程提供虚拟的 CPU. 

CPU 虚拟化涉及到 context switch 的机制 (mechanism) 和 scheduling 的策略 (机制和策略通常分别用于回答 how 和 which 两类问题的答案). 

上下文切换涉及进程的关键状态. 进程的机械状态 (machine state) 包括
- 进程可以访问的内存, 其中有程序运行需要的指令和数据; 
- 寄存器状态, 比较关键的包括 PC, 栈指针 SP, 相关的帧指针 (Frame pointer); 
- 一些 I/O 状态, 比如进程当前打开的文件列表. 
这些对于进程运行重要的信息称为进程的上下文. 

上下文切换的成本不仅仅来自保存和恢复少量寄存器的操作系统操作. 程序运行时, 它们在 CPU 高速缓存,  TLB, 分支预测器和其他片上硬件中建立了大量的状态. 

- Chapter 4.3 进程创建: 操作系统如何创建以及初始化一个进程

- Chapter 4.4 进程状态: 进程有哪些状态

- Chapter 4.5 数据结构: 操作系统中通常有一个进程结构体列表来跟踪进程的状态. 有时候, 存储关于进程的信息的个体结构称为进程控制块 (Process Control Block, PCB). 

### Chapter 5 

进程 API: `fork()`, `exec()`, `wait()` etc. `fork()` 和 `exec()` 分离给了 shell 在 fork 之后, exec 之前执行代码的机会. 

信号子系统. 

Unix 工具. 

### Chapter 6 

受限直接执行 (Limited direct execution). 
- 受限要求操作系统对于用于程序拥有一定的控制权, 比如用户程序不能直接访问硬件资源. 硬件通过执行模式限制用户程序可以访问的寄存器和可以执行的指令, 操作系统通过提供通过 trap 指令实现的系统调用服务用户程序的需求. 
  - 在 x86 中, 处理器会将程序计数器、标志和其他一些寄存器推送到每个进程的内核栈 (kernel stack). 
  - 在 RICSV 中, 类似的信息会保存在进程的 trapframe 内. 
  - 用户程序使用的系统调用实际上是 trap 指令的包装函数, 从而遵循调用约定, 将参数和返回值放在正确的位置上. 

如何切换进程
- 在协作调度系统中, OS 通过等待系统调用, 或某种非法操作发生来重新获得 CPU 的控制权
- 更为一般的系统中, 硬件每个几毫秒触发时钟中断, 调用中断处理程序将控制权还给 CPU. 
[这一节涉及到了异常控制流, 详见 CSAPP. 同时 xv6 中 kernel/trap.c 程序也很有启发性. 同时系统调用和中断可以同时发生, 应该如何处理如何处理]

操作系统可能简单地决定，在中断处理期间禁止中断. 不过禁用中断时间过长可能导致丢失中断, 这 (在技术上)是不好的. 

## 调度

### Chapter 7
饥饿是很多策略容易出现的问题

### Chapter 8 

### 多处理器调度

缓存一致性的问题
### RTOS 

## 虚拟内存


# 并发
 
进程, I/O 多路复用, 多线程是并发的一些形式

共享数据(counter 的例子)
- 竟态条件(race condition): 多个执行线程大致同时地访问共享资源, 比如一个内存地址被并发地访问, 且至少有一个是写. 会导致 either a lost update (if the accesses are writes) or a read of an incompletely-updated data structure. 
- 临界区是访问共享变量或者共享资源的代码片段, 一定不能由多个线程同时执行. 想要对临界区实现互斥 (mutual exclusion). 

sleep/wakeup 机制

## Chapter 27 线程 API 
POSIX 线程库 Pthread 提供了应用程序并发的接口

一些注意事项:
- 锁和条件变量需要初始化. 
- 线程之间共享数据，值要在堆 (heap) 或者其他全局可访问的位置, 不要使用线程栈上的返回值. 
- 线程之间总是通过条件变量发送信号, 而不要用标记变量来同步.

**不变性**: 不变性是并发编程中理解多线程环境执行一致性和正确性最重要的概念. 比如 `head` 总是应该指向链表的头节点, 在对链表进行头插法的时候, 不变性会被短暂的打破, 这时候就会引发潜在的 bug. 当我们说锁保护某个贡献资源或者数据, 我们实际上指的是保护一组在其上应用的不变量. 

## Chapter 28. Lock

锁为程序员提供了最小程度的调度控制, 强制并发操作序列化执行, 从而保证只有一个线程活跃在临界区, 为此 Phread 锁也成为 mutex(互斥量). 

评价锁: 
- 有效性, 
- 公平性, 
- 性能: 使用锁之后增加的时间开销。一种是没有竞争的情况, 只有一个线程抢锁, 释放锁的开销, 二是一个 CPU 上多个线程竞争, 性能如何? 最后是多个 CPU, 多个线程竞争时的性能. 

锁的实现依赖于硬件以及操作系统的支持.

- Chapter 28.5 控制中断: 在临界区关闭中断.  
  - 缺点: 中断丢失, 不适应多处理器系统 ... 
- DEKKER 算法和 PETERSON 算法
  - 松散内存一致性模型 (Relaxed Memory Consistency Model): 允许处理器对内存操作顺序进行重排序的内存模型
  - 特定条件下需要使用内存屏障保证内存操作的顺序. 可以通过内联汇编或者GCC 内置函数来实现. (GCC 中的 `__sync_synchronize`)

### 测试并设置指令(原子交换)

test and set 的同步原语（synchronization primitive）可以用于实现自旋锁 (spin lock). 

以下 C 代码返回 old_ptr 指向的旧值，同时更新为 new 的新值, 模拟了 test and set 指令的行为:
``` c
int TestAndSet(int *old_ptr, int new) {
    int old = *old_ptr;
    *old_ptr = new;
    return old;
}
```
重要的是, 这个操作是原子的, 即不会被中断. 

自旋锁的正确工作需要抢占式的调度

+ 在 RISC-V 中, 可以使用 amoswap 交换寄存器和内存中的值. 

### 比较并交换

某些系统中提供了比较并交换 (compare-and-swap, CAS) 的硬件原语, 可以使用以下 C 代码来模拟:
``` c
int CompareAndSwap(int *ptr, int expected, int new) {
    int actual = *ptr;
    if (actual == expected)
        *ptr = new;
    return actual;
}
```

在 RISC-V 中, 可以使用 lr/sc (load-reserved/store-conditional) 指令组合来实现称为原子比较并交换 (atomic compare-and-swap, AMO) 的操作.
- LR.W (Load-Reserved Word): 从内存中读取一个值，并且在硬件中标记这个地址为“保留”状态.
- SC.W (Store-Conditional Word): 尝试将一个新值写入到被“保留”状态的地址中. 如果这个地址自 LR.W 指令执行后没有被其他核或线程修改过，那么写入操作成功，返回 0；如果地址已被修改，则写入失败，返回非 0 值.

在 x86 中, 可以使用 cmpxchg 指令来实现 CAS 操作.
```
int compare_and_swap(int *ptr, int old, int new) {
    int old_value;
    __asm__ volatile (
        "lock cmpxchgl %2, %1"  // lock前缀确保多核下的原子性
        : "=a" (old_value), "+m" (*ptr)
        : "r" (new), "0" (old)
        : "memory"
    );
    return old_value;
}
```
- CMPXCHG 指令会比较寄存器 EAX 的值与指定内存位置的值. 如果它们相等, 则将另一个寄存器的值写入该内存位置; 否则, 它会将内存位置的值加载到 EAX 中. 
- `lock` 前缀确保了这个操作在多核下的原子性.

内存屏障

可重入性质/锁
## 条件变量

线程可以使用条件变量（condition variable）来等待一个条件变成真. 条件变量是一个显式队列, 当某些执行状态（即条件, condition）不满足时, 线程可以把自己加入队列, 等待（waiting）该条件. 另外某个线程, 当它改变了上述状态时, 就可以唤醒一个或者多个等待线程（通过在该条件上发信号）, 让它们继续执行.

调用 signal 时持有锁（ hold the lock when calling signal）
wait 调用总是假设你调用它时已经持有锁、调用者睡眠之前会释放锁以及返回前重新持有锁。
总是使用 while 循环来检查条件, 而不是 if 语句.
# Chapter 31. Semaphores

Dijsktra 因 1. 非负权重图中的最短路算法, 2. 关于结构化编程 "Go To Statement Considered Harmful", 3. 提出了信号量的同步原语而闻名. 

信号量可以用作锁和条件变量. 信号量有两个操作: `P` 和 `V`. `P` 操作会将信号量的值减一, 如果值小于 0, 则阻塞. `V` 操作会将信号量的值加一, 如果有线程在等待, 则唤醒一个线程. 信号值通常可以在某种程度上表明资源的多少. 

生产者-消费者问题:

读者-写者问题:

哲学家就餐问题:

信号量可以通过锁和条件变量来实现, 比如下面的代码定义了一个名为 Zem_t 的信号量:
``` c
typedef struct {
    int value;
    pthread_mutex_t lock;
    pthread_cond_t cond;
} Zem_t;
``` 

# Chapter 32. Common Concurrency Problems

## Non-Deadlock Bugs
两种非死锁缺陷: 违反原子性 (atomicity violation) 和 错误顺序 (order violation bugs). 前者可以定义为: The desired serializability among multiple memory accesses is violated. 即代码的本意是原子的, 但是执行中没有强制实现原子性. 后者可以定义为: The desired order between two (groups of) memory accesses is violated. 即两个内存的访问顺序被打破了. 可以通过条件变量强制顺序. 

## Deadlocks

例如, 当线程 1 持有锁 L1, 正在等待另外一个锁 L2, 而线程 2 持有锁 L2, 却在等待锁 L1 释放时, 死锁就产生了.

发生死锁有以下这样一些原因:
- 大型代码库中组件之间的循环依赖. 
- 封装. 考虑以下的代码
``` java
Vector v1, v2;
v1.AddAll(v2);
```
为了实现线程安全, 需要获得 `v1` 和 `v2` 的锁. 假设这个方法中先获得 `v1` 的锁, 然后获得 `v2` 的锁. 假如有另一个线程执行 `v2.AddAll(v1);` 就可能产生死锁. 这个问题可以通过按照锁的内存地址顺序来获得锁来解决. 

死锁的产生有四个必要条件: 
- 互斥: 线程要求独占资源.
- 持有并等待: 线程持有资源并等待另一个资源.
- 非抢占: 线程获得的资源不能被抢占.
- 循环等待: 一组线程互相等待对方持有的资源.

预防死锁 Prevention:
- 循环等待: 指定一个获取锁的全序或者偏序, 保证线程按照这个顺序获取锁.
- 持有并等待: 可以通过原子地抢占所有资源来解决. 可以理解成给给锁本身加锁.
- 非抢占: 如果抢不到锁, 就释放已经获得的锁. 这种解决办法有可能导致活锁(livelock), 即线程不断释放和获取锁, 但是没有进展. 这一点可以通过引入随机性来解决, 即线程在释放锁后等待一个随机时间再尝试获取锁. 此外, 如果某个锁被封装一个函数内部, 这种方法就不适用了; 如果代码在中途获取了某些资源, 必须要确保这些资源被释放
- 互斥: 尝试通过完全避免互斥来解决. Herlihy 提出了无等待(wait-free)的数据结构, 通过硬件指令, 比如 CAS, 来实现.

Deadlock Avoidance via Scheduling: 
- 通过调度来避免死锁. 需要了解全局的信息, 比如线程的资源需求, 线程的优先级等. 将有可能导致死锁的线程调度在不同的时间片上运行.

Check and Recovery:
- 检查死锁并恢复. 允许死锁发生, 但是检测到死锁后, 通过一些方法来恢复. 比如关机重启, 或者杀死某些线程. 数据库系统通常会定期运行一个死锁检测器, 构建一个资源图, 然后检查是否有环. 

最后, 或许可以使用 Mapreduce 之类的新的并发变成模型来避免死锁.

# File system

磁盘的划分: boot, superblock, log, bitmap, inodes, data 

buffer cache: The buffer cache has two jobs: (1) synchronize access to disk blocks to ensure that only one copy of a block is in memory and that only one kernel thread at a time uses that copy; (2) cache popular blocks so that they don’t need to be re-read from the slow disk. 
bread ,bwrite, brelease.
Per-buf sleep lock. 