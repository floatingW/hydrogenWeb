# hydrogenWeb

## 简介

hydrogen，即氢气。hydrogenWeb是一个linux下的轻量、高性能Web Server。使用C++17/CMake/GTest，无其他依赖。

## 完善

1. 增加epoll的支持
2. 增加用于处理CPU密集型任务的计算线程池
3. 异步日志系统（现阶段直接使用spdlog）
4. 数据库支持
5. 增加更加完善的性能测试
6. 覆盖更多路径的单元测试
7. 完善连接管理

## 测试对比

与其他网络库的对比，请看[性能测试](./BENCHMARK.md)

## 设计思路

细节部分已移至[这里](./DESIGN.md)。

## 项目目的

[这里](./PURPOSE.md)简述了一些想法和目的。

## 参考

1. [CMU ICS course](http://www.cs.cmu.edu/afs/cs/academic/class/15213-s20/www/index.html)
2. _Computer Systems: A Programmer's Perspective, 3/E (CS:APP3e)_
3. 《Linux多线程网络编程》
4. _Unix Network Programming: The Sockets Networking API_
5. _The Linux Programming Interface_
6. _The C Programming Language, 2/E_
7. _C++ Primer, 5/E_
8. [muduo - Event-driven network library for multi-threaded Linux server in C++11](https://github.com/chenshuo/muduo)
9. [libev - Full-featured high-performance event loop loosely modelled after libevent](https://github.com/enki/libev)
10. [Netty project - an event-driven asynchronous network application framework](netty.io)
11. [Tiny HTTPd](https://sourceforge.net/projects/tinyhttpd)