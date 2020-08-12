/*
 * File: HttpServer_test.cpp
 * -------------------------
 * @author: Fu Wei
 *
 */

#include "network/http/HttpServer.hpp"
#include "network/http/HttpRequest.hpp"
#include "network/http/HttpResponse.hpp"
#include "network/EventLoop.hpp"
#include "network/InetAddr.hpp"
#include "system/unixUtility.hpp"
#include "config.h"

#include <iostream>
#include <unordered_map>
//#include <fstream>
//#include <sstream>

#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>

#include <spdlog/spdlog.h>
#include <sqlite3.h>

using namespace std;

unordered_map<string, string> mimeTypes;

namespace detail
{
    int openDb(const string& dbPath, sqlite3** ppdb)
    {
        int ret = sqlite3_open(dbPath.c_str(), ppdb);
        if (ret)
        {
            spdlog::error("Can't open database: {}", sqlite3_errmsg(*ppdb));
        }
        else
        {
            spdlog::info("Opened Database");
        }

        return ret;
    }

    int execSql(sqlite3* pdb,
                const string& sql,
                int (*callback)(void*, int, char**, char**),
                void* data)
    {
        char* errmsg;
        int ret = sqlite3_exec(pdb, sql.c_str(), callback, data, &errmsg);
        if (ret != SQLITE_OK)
        {
            spdlog::error("Can't do SQL: {}, errmsg: {}", sql, errmsg);
            sqlite3_free(errmsg);
        }
        else
        {
            spdlog::info("Operation OK");
        }

        return ret;
    }

    int doSql(sqlite3* pdb, const string& sql)
    {
        sqlite3_stmt* stmt;
        const char* pointer;
        int ret = sqlite3_prepare_v2(pdb, sql.c_str(), sql.length(), &stmt, &pointer);
        if (ret == SQLITE_OK)
        {
            ret = sqlite3_step(stmt);
            sqlite3_finalize(stmt);
        }
        return ret;
    }

    void fillMimeType(unordered_map<string, string>& mimeTypes)
    {
        mimeTypes[".html"] = "text/html";
        mimeTypes[".mp4"] = "video/mpeg";
        mimeTypes[".png"] = "image/png";
        mimeTypes[".jpg"] = "image/jpeg";
        mimeTypes[".jpeg"] = "image/jpeg";
        mimeTypes[".gif"] = "image/gif";
        mimeTypes[".mp3"] = "audio/mpeg";
        mimeTypes["default"] = "text/html";
    }

    void responseOK(HttpResponse* resp, const string& contentType)
    {
        resp->setStatusCode(HttpResponse::OK200);
        resp->setStatusMsg("OK");
        resp->setContentType(contentType);
        resp->addHeader("Server", "HydrogenWeb");
    }

    void response404(HttpResponse* resp)
    {
        resp->setStatusCode(HttpResponse::NOTFOUND404);
        resp->setStatusMsg("Not Found");
        resp->setContentType("");
        resp->addHeader("Server", "HydrogenWeb");
        resp->setCloseConnection(true);
        resp->setBody("404 NOTFOUND");
    }

    void response500(HttpResponse* resp)
    {
        resp->setStatusCode(HttpResponse::INTERNAL500);
        resp->setStatusMsg("Internal Server Error");
        resp->setContentType("");
        resp->addHeader("Server", "HydrogenWeb");
        resp->setCloseConnection(true);
        resp->setBody("500 INTERNAL ERROR");
    }
}

int fillData(void* data, int argc, char** argv, char** azColName)
{
    char buf[256];
    auto resp = static_cast<HttpResponse*>(data);
    resp->appendToBody("<tr>");

    for (int i = 0; i < argc; ++i)
    {
        snprintf(buf, sizeof buf, "<td>%s</td>", argv[i]);
        resp->appendToBody(buf);
    }
    resp->appendToBody("</tr>");

    return 0;
}

int checkPassword(void* data, int argc, char** argv, char** azColName)
{
    auto pPw = static_cast<string*>(data);
    if (*pPw == string(argv[0]))
    {
        spdlog::info("password correct");
        return 0;
    }
    else
    {
        spdlog::info("password incorrect");
        return -1;
    }
}

//int signup(void* data, int argc, char** argv, char** azColName)
//{
//    auto pair = static_cast<string**>(data);
//    if (argc == 0)
//    {
//    }
//    else
//    {
//    }
//}

void serveFile(const string& path, HttpResponse* resp, const string& mimeType)
{
    int fd = ::open(path.c_str(), O_RDONLY);
    struct stat st;
    auto fs = ::fstat(fd, &st);
    if (fd < 0)
    {
        spdlog::info("serveFile:open {} failed", path);
        detail::response500(resp);
        return;
    }
    auto map = ::mmap(
        nullptr, static_cast<size_t>(st.st_size), PROT_READ, MAP_PRIVATE, fd, 0);
    resp->setBody(map, st.st_size);
    munmap(map, st.st_size);
    detail::responseOK(resp, mimeType);
}

void serveStatic(const string& path, HttpResponse* resp)
{
    string suffix = path.substr(path.find('.'), path.length() - 1);
    string mimeType;
    if (mimeTypes.find(suffix) != mimeTypes.end())
    {
        mimeType = mimeTypes[suffix];
    }
    else
    {
        mimeType = mimeTypes["default"];
    }
    string filePath;
    if (mimeType == "text/html")
    {
        /** html files */
        filePath = htmlPath + path;
    }
    else
    {
        /** other files */
        filePath = objectPath + path;
    }

    serveFile(filePath, resp, mimeType);
    resp->setCloseConnection(true);
}

void onRequest(const HttpRequest& req, HttpResponse* resp)
{
    /** print headers and body */
    std::cout << "Headers " << req.methodString() << " " << req.path() << std::endl;
    const auto& headers = req.headers();
    for (const auto& header : headers)
    {
        std::cout << header.first << ": " << header.second << std::endl;
    }
    const auto& bodys = req.bodys();
    for (const auto& body : bodys)
    {
        std::cout << body.first << ": " << body.second << std::endl;
    }

    const string& path = req.path();
    if (path == "/" || path.find('.') != string::npos)
    { /** static */
        if (path == "/" || path.empty())
        {
            serveStatic("/home.html", resp);
        }
        else
        {
            serveStatic(path, resp);
        }
    }
    else
    { /** dynamic */
        if (path == "/time")
        {
            detail::responseOK(resp, "text/html");

            Timestamp now = Timestamp::now();
            resp->setBody("<html><head><title>Welcome!</title></head>");
            resp->appendToBody("<body><h1>Long time no see!</h1>");
            resp->appendToBody("<p>Now is " + now.toString() +
                               " seconds from 1970/00/00 00:00:00"
                               "</p>");
            resp->appendToBody("<p>" +
                               to_string(now.toSec() / (60 * 60 * 24)) +
                               " days</p>");
            resp->appendToBody("<p>about " +
                               to_string(static_cast<long>(now.toSec() / (60 * 60 * 24) / 30.5)) +
                               " months</p>");
            resp->appendToBody("<p>about " +
                               to_string(static_cast<long>(now.toSec() / (60 * 60 * 24) / 30.5 / 12)) +
                               " years</p>");
            resp->appendToBody("</html>");
        }
        else if (path == "/booklist")
        {
            detail::responseOK(resp, "text/html");

            resp->setBody("<html><head><title>Book list!!</title></head>");
            resp->appendToBody("<table border=\"1\">");

            sqlite3* pdb;
            if (detail::openDb(databasePath, &pdb))
            {
                /** open database failed */
                detail::response500(resp);
                return;
            }

            // fill booklist
            string sql = "SELECT * from booklist";
            if (detail::execSql(pdb, sql, fillData, static_cast<void*>(resp)) != SQLITE_OK)
            {
                /** execSql error */
                detail::response500(resp);
                return;
            }
            resp->appendToBody("</table>");
            resp->appendToBody("</html>");
            sqlite3_close(pdb);
        }
        else if (path == "/signup")
        {
            sqlite3* pdb;
            if (detail::openDb(databasePath, &pdb) != SQLITE_OK)
            {
                /** open database failed */
                detail::response500(resp);
                return;
            }

            string sql = "SELECT * from users WHERE username=\'" +
                         req.getBody("username") + "\'";
            int ret = detail::doSql(pdb, sql);
            if (ret == SQLITE_DONE)
            {
                sql = string("INSERT INTO users (username, password) ") +
                      "VALUES (\'" +
                      req.getBody("username") +
                      "\', \'" +
                      req.getBody("password") +
                      "\')";
                if (detail::execSql(pdb, sql, nullptr, nullptr) != SQLITE_OK)
                {
                    spdlog::error("insert error");
                }
                else
                {
                    spdlog::info("insert new user OK!");
                    serveStatic("/success.gif", resp);
                }
            }
            else
            {
                if (ret == SQLITE_ROW)
                {
                    spdlog::info("username existed");
                    serveStatic(req.path() + "_username.html", resp);
                }
                else
                {
                    spdlog::error("insert error");
                }
            }

            sqlite3_close(pdb);
        }
        else if (path == "/signin")
        {
            sqlite3* pdb;
            if (detail::openDb(databasePath, &pdb) != SQLITE_OK)
            {
                /** open database failed */
                detail::response500(resp);
                return;
            }

            string sql = "SELECT * from users WHERE username=\'" +
                         req.getBody("username") + "\'";
            int ret = detail::doSql(pdb, sql);
            if (ret == SQLITE_DONE)
            {
                /** no such user */
                spdlog::info("no such user");
            }
            else
            {
                if (ret == SQLITE_ROW)
                {
                    /** user existed */
                    sql = "SELECT password from users WHERE username=\'" +
                          req.getBody("username") + "\'";
                    string password = req.getBody("password");
                    if (detail::execSql(pdb,
                                        sql,
                                        checkPassword,
                                        static_cast<void*>(&password)) != SQLITE_OK)
                    {
                        spdlog::error("do checkPW error or PW incorrect");
                        serveStatic(req.path() + "_pw.html", resp);
                    }
                    else
                    {
                        spdlog::info("do checkPW OK");
                        serveStatic("/success.gif", resp);
                    }
                }
                else
                {
                    spdlog::error("select password error");
                }
            }

            sqlite3_close(pdb);
        }
        else
        {
            detail::response404(resp);
        }
    }
}

int main(int argc, char* argv[])
{
    if (argc < 2)
    {
        cerr << "Usage: " << argv[0] << "<port> [numThreads: default=0]\n";
        exit(0);
    }
    int port = 23456;
    int numThreads = 0;
    if (argc == 3)
    {
        port = stoi(argv[1]);
        numThreads = stoi(argv[2]);
    }
    else if (argc == 2)
    {
        port = stoi(argv[1]);
    }
    else
    {
        cerr << "Usage: " << argv[0] << "<port> [numThreads: default=0]\n";
        exit(0);
    }
    spdlog::set_level(spdlog::level::err);

    detail::fillMimeType(mimeTypes);
    EventLoop loop;
    HttpServer server(&loop, InetAddr(port), numThreads);
    server.setHttpCallback(onRequest);
    server.run();
    loop.loop();
    return 0;
}
