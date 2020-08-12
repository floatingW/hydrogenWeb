# hydrogenWeb

## 简介

hydrogen，即氢气。hydrogenWeb是一个linux下的轻量、高性能Web Server，内部使用自建的网络库。项目基于C++17/CMake/GTest开发。

## 快速开始

### 构建

首先确认安装如下依赖：

* gcc 7.x/clang 8.x
* CMake 3.10.x
* GTest 1.10
* SQLite 3

```bash
mkdir build
cd build
cmake ..
make
```

### 运行demo

```bash
./build/application/hydrogenWeb_demo <port> [numThreads: default=0]
```

## demo基本功能演示

1. 注册/登录

   ![signup][signup_gif]

   ![signin][signin_gif]

2. 访问音频资源（文件大小10MB）

   ![audio][audio_gif]

3. 访问视频资源（文件大小347MB）

   ![video][video_gif]

4. 访问图片资源（文件大小11MB）

   ![image][image_gif]

5. 时间服务

   ![dynamic][time_gif]

6. 读取书单（2万条记录）

   ![dynamic][booklist_gif]

[signup_gif]: https://raw.githubusercontent.com/fuweiagn/imgs/master/hydrogen/signup.gif
[signin_gif]: https://raw.githubusercontent.com/fuweiagn/imgs/master/hydrogen/signin.gif
[audio_gif]: https://raw.githubusercontent.com/fuweiagn/imgs/master/hydrogen/audioplayer.gif
[video_gif]: https://raw.githubusercontent.com/fuweiagn/imgs/master/hydrogen/videoplayer.gif
[image_gif]: https://raw.githubusercontent.com/fuweiagn/imgs/master/hydrogen/image.gif
[time_gif]: https://raw.githubusercontent.com/fuweiagn/imgs/master/hydrogen/time.gif
[booklist_gif]: https://raw.githubusercontent.com/fuweiagn/imgs/master/hydrogen/booklist.gif

## 测试对比

与其他网络库的对比，请看[性能测试](./BENCHMARK.md)。

## 设计思路

大体上的思路请看[这里的设计思路](./DESIGN.md)。具体细节的分析请看[细节探讨](./DETAIL.md)。

## 项目目的

[这里](./PURPOSE.md)简述了一些想法和目的。

## 路线图1.0

1. 为Linux增加epoll的支持，在某些应用场景下能有比poll更好的性能
2. 增加用于处理CPU密集型任务的计算线程池，为计算型服务提供便利
3. 异步日志系统（现阶段直接使用spdlog输出日志）
4. 更加完善的数据库支持（初步支持sqlite3）
5. 增加更加完整的性能测试（现阶段由于测试用的电脑无法提供比较多的核心，无法测试双线程以上的应用场景）
6. 覆盖更多路径的测试
7. 完善连接管理
8. 支持使用配置文件搭建服务器

## 问题反馈

出现的问题会反馈在Issues中。

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