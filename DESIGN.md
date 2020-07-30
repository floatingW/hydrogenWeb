# 设计思路
## Unix下的I/O模型
参考 _Unix Network Programming(vol.1)_ （以下简称UNP）总结以下5种I/O模型：
1. blocking IO

   系统调用要知道数据到达或出现错误才返回（错误通常是被信号中断），这期间都属于被阻塞状态

2. nonblocking IO

   非阻塞表示，请求IO操作时，如果数据还未到达，不把进程投入睡眠，而是直接返回错误（比如EWOULDBLOCK），一旦有数据准备好了，则成功返回。我们可以处理数据了。Linux从2.6.25开始，创建fd的系统调用都增加了NONBLOCK选项，作用就是开启非阻塞IO，之前的fd默认都是阻塞的。

3. IO多路复用模型

   进程（线程）阻塞在select/poll等系统调用上，一旦有fd可读，则返回，我们可以处理这个fd。优势在于可以等待多个fd准备就绪。一次性对这些fd进行处理。

4. 信号驱动IO

   即通过内核发送signal中断进程，通知我们某个fd准备就绪了，我们通过安装好的signal handler处理这个fd。这样也可以达到非阻塞的效果。

5. 异步IO

   某些函数的工作机制可以是异步的。异步IO的意思是：函数告知内核启动某个IO操作，内核完成该IO操作后，再通知我们。与signal的区别是，signal告诉我们什么时候可以开始，异步IO模型是内核通知我们IO操作已经完成了。当然，这个通知的形式也是signal。

前四种都是同步的，IO操作都会阻塞。一个输入操作通常包括：等待数据准备好，从内核向进程拷贝数据。对于一个socket的输入操作，那么就是等待数据从网络到达，拷贝进内核的缓冲区。然后把数据从内核缓冲区拷贝到应用进程的缓冲区。

其中IO多路复用，复用的是线程，可以在同一个线程里，处理多个连接的输入输出事件。很多网络库内部实现都采用了这种IO模型。

![multiplexing](https://en.wikipedia.org/wiki/File:Multiplexing_diagram.svg)

_UNP_ 提到几种IO复用的场景：
1. 客户需要处理多个fd，必须使用IO复用。
2. TCP服务器既要监听listenfd以接收新连接，又要处理已连接fd进行交互，那么一般也要使用IO复用。
3. 服务器即支持TCP又要支持UDP，一般要使用IO复用。
4. 服务器需要处理多个服务或协议，一般要用IO复用。

可见IO复用非常灵活，适应多种场合。并且减少了对每个关注的fd的都进行轮询的开销。

## 并发编程模型
借用《linux多线程服务端编程》里对常见几种并发编程模型的归纳图：

![models][img1]

0. 不是并发服务器，属于迭代服务器，没有进程控制。每次服务一个连接。适合write-only的短连接服务。
1. 对于每个连接fork一个进程进行服务，适用于计算工作量远大于fork开销的服务器程序。_UNP_ 30.5中介绍。
2. 传统的Java网络编程方案，对于每个连接新建一个线程进行服务。开销比fork小很多，但依然不适合太大量的并发，程序处理能力会随着连接数增加而下降而不是维持一个稳定的峰值。_UNP_ 30.10中介绍。
3. 1的基础上优化，预先创建一些进程，从进程池中选用进程进行服务，减少开销。_UNP_ 30.6-30.9中介绍了4中预派生子进程的编程模式，区别在于锁的使用和描述符的传递策略的不同。
4. 2的优化，预先创建一些线程，从线程池中选用线程进行服务，减少开销。3、4方案都是apache httpd长期使用过的方案。_UNP_ 30.11-30.12介绍两种预线程模式，区别在于是每个线程分别accept还是单独一个线程accept。

_UNP_ 中总结以上5种服务器设计范式：

![models][img2]

以上5种都是阻塞式的，平时会阻塞在系统调用上（read/write），如果阻塞在read上，就不能往回发数据或者从stdin读数据，比如 _Computer Systems: A Programmer's Perspective_ 中echo server的例子。如何解决呢？除了多个线程、进程分别处理读、写任务外，还有就是使用IO multiplexing，比如CSAPP中使用IO多路复用实现并发事件驱动改善echo server，属于非常简化的事件驱动编程模式。利用IO多路复用机制进行分发配合事件驱动，将业务逻辑代码的回调函数注册到框架中，这种方式，1995年被Douglas C. Schmidt在  [_An Object Behavioral Pattern for Demultiplexing and Dispatching Handles for Synchronous Events_][1] 中总结为Reactor模式。reactor模式就是将IO事件的分发和用户实现的业务逻辑分开，处理网络连接的网络库部分无需变动，对于服务器程序的编写意义重大。reactor拥有一个事件循环，负责处理IO，处理IO的线程称作IO线程，进行socket的读写、回调用户代码。现在流行的网络库libevent/muduo/libev/Netty/twisted等就采用了reactor模式。

5. 最基本的单线程reactor，因为没有发挥多核的优势，只适合IO密集的应用，不适合CPU密集的以用。相比2、3处理消息的延迟稍高，因为比之多了poll系统调用。
6. 5基础上的优化，单独的IO线程进行连接的事件分发，对于每个请求新建线程进行具体的业务处理。缺点跟2类似，线程过多的话容易造成操作系统调度的负担过重。
7. 6的优化，主要是固定每个连接的请求只能交给特定一个工作线程处理，保证返回结果的顺序。与6的另一个区别是对于一个连接，因为可能会有很多个请求，对于6，可以更快全部处理完，对于7，只能使用单个核心。无法说孰优孰劣，要考虑公平性和突发性能的之间的权衡。
8. 6基础上，固定线程数量。线程池另一个作用是用来执行阻塞操作，可以让某些线程执行诸如数据库操作、或其他阻塞的系统调用。
9. one loop per thread（muduo原生的多线程IO模型），一个main reactor负责accept连接，然后将连接分派到多个sub reactor之一（muduo用round-robin方式选择），那个连接的任务请求就只由那个sub reactor的线程完成。连接发送到sub reactor后，小规模计算可以直接在IO线程完成直接发回，降低延迟。这样少了thread pool中线程的进出，也就是两次线程上下文切换的开销。
10. Nginx的方案，如果连接之间没有交互，就不担心繁琐的IPC，那么也是很好的选择。而且进程之间独立，可以热升级。
11. 8、9的混合，既有多个reactor处理IO，又有thread pool进行计算任务。适合既有突发IO，又有突发计算。CPU利用率可以比9更高，但延迟也更大。具体应该使用一个eventloop还是多个，参考ZeroMQ的手册建议：每千兆比特每秒的吞吐量一个eventloop。如果IO带宽较小，计算量大，延迟不敏感，可以把计算全部放到thread pool中，也可以只用一个eventloop。如果连接有优先级，那么高优先级需要单独一个eventloop。

[img1]:https://raw.githubusercontent.com/fuweiagn/imgs/master/imgs/concurrent_network_programming_models.jpg?token=APOZM5NGCOO4UG6QPHXCH2K7EKOC4
[img2]:https://raw.githubusercontent.com/fuweiagn/imgs/master/imgs/UNP_models.jpg?token=APOZM5N4HFIS43ZFRTLQTGC7EKOXI
[1]:http://www.dre.vanderbilt.edu/~schmidt/PDF/reactor-siemens.pdf