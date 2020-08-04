# 性能测试

## 单线程echo服务吞吐量测试

### 测试方法

客户端使用ab进行单线程的不同并发数的访问，服务端echo一条14字节的字符串，平均每秒完成请求数越高代表吞吐量越大。现代CPU单线程单TCP连接都足以跑满千兆网，为了避免千兆网对吞吐量测试的限制，我们在在单机环境下进行测试。

### 测试环境

一台虚拟机系统：

* 虚拟机软件：VMware Workstation Player 15.5.5
* Linux kernal：5.7.9
* CPU: 4 cores/4.4GHz
* MEM: 8G
* Nginx 1.18.0-1
* muduo 2.0.2
* ab (apache 2.4.43-1)

### 运行代码

muduo使用的测试代码来自《Linux多线程服务器编程》，作了少许修改，这里省去了一些无关代码：

```cpp
void onRequest(const HttpRequest& req, HttpResponse* resp)
{
  if (req.path() == "/")
  {
      //省略部分无关代码
  }
  else if (req.path() == "/hello")
  {
    resp->setStatusCode(HttpResponse::k200Ok);
    resp->setStatusMessage("OK");
    resp->setContentType("text/plain");
    resp->addHeader("Server", "Muduo");
    resp->setBody("hello, world!\n");
  }
  else
  {
    resp->setStatusCode(HttpResponse::k404NotFound);
    resp->setStatusMessage("Not Found");
    resp->setCloseConnection(true);
  }
}

int main(int argc, char* argv[])
{
  int numThreads = 0;
  if (argc > 1)
  {
    numThreads = atoi(argv[1]);
  }
  EventLoop loop;
  HttpServer server(&loop, InetAddress(8000), "dummy");
  server.setHttpCallback(onRequest);
  server.setThreadNum(numThreads);
  server.start();
  loop.loop();
}
```

Nginx使用OpenResty的[echo-nginx-module](https://github.com/openresty/echo-nginx-module)，运行下面这个简单的echo服务器：

```nginx
#user nobody;
worker_processes 1; # 单线程

events {
        worker_connections 10240;
}

http {
        include /usr/local/openresty/nginx/conf/mime.types;
        default_type application/octet-stream;

        access_log off; # 日志
        sendfile on;
        tcp_nopush on;
        keepalive_timeout 65;

        server {
            listen 8000;
            server_name localhost;

            location / {
                root html;
                index index.html index.htm;
            }

            location /hello {
                default_type text/plain;
                echo "hello, world!";
            }
        }
}
```

### 运行测试

客户端运行ab，总共10万请求数，开启长连接、socket收到错误不退出连接、并发数C范围[1,5,10,50,100,500,1000]，每个并发数测试10次：

```bash
 ab -n 100000 -k -r -c C localhost:8000/hello
```

### 测试结果

对于三者，从5/10并发数到最高并发数的情况下，client线程的CPU占用率都没有超过71%，并且server线程CPU占用率趋于一个峰值，所以可以认为client线程能够给予server线程压力。

|并发数|1|5|10|50|100|500|1000|
|---|---|---|---|---|---|---|---|
|hydrogen|10129|32262|32484|32446|31667|29889|29397|
|muduo|10989|38101|35475|34840|36883|34506|34220|
|对比muduo|-7.8%|-15.3%|-8.4%|-6.9%|-14.1%|-13.4%|-14.1%|
|Nginx|9365|27258|27740|26112|25067|23381|22129|
|对比Nginx|8.2%|18.4%|17.1%|24.3%|26.3%|27.8%|32.8%|

![result][img0]

单线程echo服务的吞吐量测试：hydrogen对比muduo平均落后11.4%，对比Nginx平均领先22.1%。

[img0]: https://raw.githubusercontent.com/fuweiagn/imgs/master/hydrogen/single_thread_echo_throughput_test.jpg
