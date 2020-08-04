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

![multiplexing][img0]

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

其中对于现阶段来说比较实用的有这几种方案：

|#|模型名称|
|---|---|
|2|one thread per connection|
|5|reactor|
|8|reactor+thread pool|
|9|one loop per thread|
|11|one loop per thread + thread pool|

## 编程模型选择

考虑上5种模型，各个方案优劣如下：

方案2：上面已经叙述过，不适合太大量的并发，程序处理能力会随着连接数增加而下降而不是维持一个稳定的峰值
方案5：单线程Reactor模式，只适合IO密集，不适合计算密集。
方案8：相当于在单线程Reactor模式基础上增加一个线程池，用来执行一些阻塞、费时操作或者计算。
方案9：相当于多个单线程的Reactor，能划分不同优先级的连接，适应性较强（是muduo原生的多线程IO模型）
方案11：方案8、方案9的混合，具有多个Reactor处理IO，thread pool处理计算。相比起方案9，资源利用率可以更高，能更好地应对突发请求，但延迟也会比方案9稍大。

对于我们的HTTP服务器，需要适应性强，遇到突发的并发访问也不能造成拥堵。综合考虑选择方案11，也就是one loop per thread + thread pool，相应的也只是会增加一点延迟，在不需要低延迟服务的情况下完全可以接受。

> one loop per thread is usually a good model. Doing this is almost never wrong, sometimes a better-performance model exists, but it is always a good start. - [libev][2]

one loop per thread 是指每个线程一个Reactor（主要包含一个event loop），event loop用于IO multiplexing和执行non-blocking IO 和定时器任务。thread pool则从任务队列或生产者消费者队列中获取任务。

## Reactor 的 event loop

Reactor模式基本上就是通过IO multiplexing等待并发事件，以event-driven和事件回调的方式完成业务逻辑。而non-blocking IO与IO multiplexing几乎都是一起使用，因为blocking IO会导致无法接收新的连接，并且没有人会使用busy polling来检查non-blocking IO是否完成，这样对CPU周期也会是极大的浪费。由于Reactor模式的这种特性，事件的回调也必须是non-blocking的，不然会影响新连接的进入和已有事件的进行。Reactor是one loop per thread的基础，先简单介绍其实现。

### EventLoop

EventLoop就是one loop per thread的那个loop，每个线程只能有一个，并且是只属于一个线程，在哪个线程创建，就在哪个线程运行。EventLoop会执行事件循环，每次循环都会检查fd集合中是否有事件发生。

### EventLoopThread

EventLoopThread负责在一个新线程中创建EventLoop，是one loop per thread的具体体现。方便对各个EventLoop进行优先级划分。

### Channel

Channel是Reactor最核心的部分，也就是负责事件分发。每个Channel关联一个fd，并且只属于某个EventLoop（也就是每个Channel自始至终只会在某个固定的IO线程中工作）。每当关联的fd上有事件发生时，Channel会将不同类型的事件分发到不同的回调函数。由于Channel只会在固定的IO线程工作，所以是线程安全的，因为对于其他线程而言它根本就是不可见的。

### Poller

Poller就是IO multiplexing机制的封装而已，对于不同的操作系统，它可以有不同的实现，Linux下可以是select/poll/epoll，solaris有/dev/poll接口，freebsd 4.1版本引入了kqueue，这些接口支持IO multiplexing。Poller是EventLoop的成员，它会检查它所属EventLoop的fd集合，监视事件的发生。同样的，由于只属于固定的某个IO线程，Poller是线程安全的，无需加锁。

### TimerQueue

传统Reactor通过控制poll接口的等待时间实现定时功能，Linux 2.6.25 新增了timerfd，与其他fd使用方式一致，代码一致性更好。可以通过设置timerfd的到期时间，实现定时器功能。TimerQueue关联一个Channel，后者关联timerfd，我们往TimerQueue里增加定时事件，timerfd到期时，Channel回调TimerQueue的handler将到期Timer一一处理。TimerQueue的线程安全性也是通过将其成员函数移动到EventLoop中执行来保证的，增加Timer这一动作会被加入EventLoop的任务队列，在单一线程中顺序执行，所以无需加锁。

## 支持TCP的Reactor网络库

到此为止已经可以通过注册sockfd进行网络通信、注册timerfd实现定时器功能了，能实现单线程对多个连接的并发服务。但对连接还没有细致的处理、fd的生命周期也没有妥善管理，实际工作中由于连接意外中断可能会发生业务上的错误。而且也没有显式的方便调用的接口来监听端口。我们还需要对Linux的socket相关的接口进行封装，方便调用，同时将sockfd进行RAII-style的封装，确保其生命周期的安全管理。

### Socket

RAII-style封装的sockfd，生命周期由其拥有者确定。由于每个sockfd都只在固定的一个IO线程里读写，所以一般用unique_ptr对其进行管理，拥有者失去其所有权后，Socket在析构时会close掉对应的sockfd，保证fd不会泄露。

### Acceptor

专门用于accept新连接，执行TCP socket的socket/bind/listen/accept这系列系统调用。内部封装了Channel，一旦listenfd上有新连接事件，Channel就回调Acceptor负责accept 的处理函数进行accept，并返回connfd。

### TcpServer

前面说到的Acceptor返回的connfd，会被TcpServer用来新建一个TcpConnection。TcpConnection是连接的抽象，而TcpServer负责管理所有的这些连接。另外，TcpServer还提供了方便的调用接口来监听特定端口，内部是通过Acceptor来完成的，后者是它的成员。

### TcpConnection

每个TcpConnection是一个TCP连接的抽象，代表着一个TCP连接的生命周期。由于其生命周期的不确定性，我们使用shared_ptr来引用它们，并由TcpServer进行管理。其内部也是由Channel进行事件分发，分别管理读/写/关闭等事件。所有的读写我们都在固定的IO线程进行，不会出现两个线程读写一个TcpConnection的情况，所以是线程安全的。

### HyBuffer

前面所论述的，IO multiplexing必须配合non-blocking IO，因为不能让线程阻塞在诸如read/write这种系统调用上，不然会阻碍select/poll/epoll监听新事件，影响新连接的建立和已有连接的读写事件处理。那么应用层的buffer是必不可少的，内部使用vector来承载字节数据。

至此已经可以通过简单的几个调用建立一个TCP服务器了。

```c++
/** 新建一个监听端口的sockaddr_in结构 */
InetAddr listenAddr(23456);
/** 创建EventLoop */
EventLoop loop;
/** 创建TcpServer， 并将其与EventLoop和端口绑定 */
TcpServer tcpServer(&loop, listenAddr);
/** 设置TcpServer的回调，这个就是我们的业务逻辑代码，
    连接建立时做什么，每次有数据到达时做什么 */
tcpServer.setConnectionCallback(connCb);
tcpServer.setMessageCallback(msgCb);
/** 开始TcpServer的监听 */
tcpServer.run();
/** 开始EventLoop的事件循环 */
loop.loop();
```

## 支持HTTP的Reactor网络库

### HttpContext

作为TcpConnection的成员，解析HTTP连接某次通信的数据，保存HTTP request到它自己的HttpRequest成员中。HttpContext作为一个连接某次通信的抽象，供HttpServer使用。

### HttpRequest

HTTP request的封装，保存着每次HTTP request的解析后的内容。

### HttpResponse

HTTP response的封装，HttpServer和用户设置的HTTP请求回调会设置HttpResponse的内容，然后通过TcpConnection发回客户端，完成一次HTTP请求的响应。

### HttpServer

TcpServer的封装，使用HttpContext/HttpResponse/HttpRequest配合用户的业务逻辑回调来响应客户端的HTTP请求。

那么现在就可以通过以下的几个调用，建立起一个HTTP服务器了。

```c++
/** 创建一个EventLoop */
EventLoop loop;
/** 创建一个HttpServer */
HttpServer httpServer(&loop, InetAddr(23456));
/** 设置HttpServer的回调，也就是我们的业务逻辑代码，
    用来响应每一次HTTP请求 */
httpServer.setHttpCallback(onRequest);
/** 开始TcpServer的监听 */
httpServer.run();
/** 开始EventLoop的事件循环 */
loop.loop();
```

[img0]: https://upload.wikimedia.org/wikipedia/commons/thumb/6/6f/Multiplexing_diagram.svg/1200px-Multiplexing_diagram.svg.png
[img1]: https://raw.githubusercontent.com/fuweiagn/imgs/master/hydrogen/concurrent_network_programming_models.jpg
[img2]: https://raw.githubusercontent.com/fuweiagn/imgs/master/hydrogen/UNP_models.jpg
[1]:http://www.dre.vanderbilt.edu/~schmidt/PDF/reactor-siemens.pdf
[2]:http://cvs.schmorp.de/libev/ev.pod#THREADS_AND_COROUTINES
