# hydrogenWeb
## 简介
hydrogen，即氢气。hydrogenWeb是一个linux下的轻量、高性能Web Server。使用C++17/CMake/GTest，无其他依赖。
## roadmap
1. 迭代服务器（以简单的echo服务程序为例）
2. 并发echo服务器（基于进程、线程、IO多路复用的实现）
3. 并发Web服务器([设计书](./DESIGN.md))
## [项目目的](./PURPOSE.md)
## 参考
1. [CMU ICS course](http://www.cs.cmu.edu/afs/cs/academic/class/15213-s20/www/index.html)
2. _Computer Systems: A Programmer's Perspective, 3/E (CS:APP3e)_
2. 《Linux多线程网络编程》
3. _Unix Network Programming: The Sockets Networking API_
4. _The Linux Programming Interface_
5. _The C Programming Language, 2/E_
6. _C++ Primer, 5/E_
7. [muduo - Event-driven network library for multi-threaded Linux server in C++11](https://github.com/chenshuo/muduo)
8. [libev - Full-featured high-performance event loop loosely modelled after libevent](https://github.com/enki/libev)
9. [Netty project - an event-driven asynchronous network application framework](netty.io)
10. [Tiny HTTPd](https://sourceforge.net/projects/tinyhttpd)