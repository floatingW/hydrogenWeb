#include "gtest/gtest.h"
#include "robustio/robustio.hpp"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string>
#include <fstream>

using namespace std;

TEST(robustio_readline, correctness_simple_cases)
{
    string path = "/home/fwei/learning/hydrogenWeb/tests/test_cases/text_simple.txt";

    int fd_rio = open(path.c_str(), O_RDONLY, 0);
    Riob riob(fd_rio);
    ASSERT_TRUE(fd_rio >= 0);

    ifstream in(path);
    for (string line; getline(in, line);)
    {
        void* usrbuf[RIO_BUFSIZE];
        riob.readlineb(usrbuf, RIO_BUFSIZE);
        string str_rio((char*)usrbuf);
        line += '\n';
        EXPECT_EQ(str_rio, line);
    }
    close(fd_rio);
    in.close();
}

TEST(robustio_readline, correctness_large_cases)
{
    string path = "/home/fwei/learning/hydrogenWeb/tests/test_cases/text_large.txt";

    int fd_rio = open(path.c_str(), O_RDONLY, 0);
    Riob riob(fd_rio);
    ASSERT_TRUE(fd_rio >= 0);

    ifstream in(path);
    for (string line; getline(in, line);)
    {
        void* usrbuf[RIO_BUFSIZE];
        riob.readlineb(usrbuf, RIO_BUFSIZE);
        string str_rio((char*)usrbuf);
        line += '\n';
        EXPECT_EQ(str_rio, line);
    }
    close(fd_rio);
    in.close();
}

TEST(robustio_readn, correctness_simple_cases)
{
    string path = "/home/fwei/learning/hydrogenWeb/tests/test_cases/text_simple.txt";

    int fd_rio = open(path.c_str(), O_RDONLY, 0);
    int fd_sys = open(path.c_str(), O_RDONLY, 0);
    ASSERT_TRUE(fd_sys >= 0);

    int count = 10;
    int bytes = sizeof(char);
    for (int n = 0; n < count; n++)
    {
        char buf_rio[RIO_BUFSIZE];
        char buf_sys[RIO_BUFSIZE];

        int cnt_rio = Rio::readn(fd_rio, buf_rio, n * bytes);
        int cnt_sys = read(fd_sys, buf_sys, n * bytes);
        EXPECT_EQ(cnt_rio, cnt_sys);

        for (int i = 0; i < n; i++)
        {
            EXPECT_EQ(buf_rio[i], buf_sys[i]);
        }
    }

    close(fd_rio);
    close(fd_sys);
}

TEST(robustio_readn, correctness_large_cases)
{
    string path = "/home/fwei/learning/hydrogenWeb/tests/test_cases/text_large.txt";

    int fd_rio = open(path.c_str(), O_RDONLY, 0);
    int fd_sys = open(path.c_str(), O_RDONLY, 0);
    ASSERT_TRUE(fd_sys >= 0);

    int count = 1000;
    int bytes = sizeof(char);
    for (int n = 0; n < count; n++)
    {
        char buf_rio[RIO_BUFSIZE];
        char buf_sys[RIO_BUFSIZE];

        int cnt_rio = Rio::readn(fd_rio, buf_rio, n * bytes);
        int cnt_sys = read(fd_sys, buf_sys, n * bytes);
        EXPECT_EQ(cnt_rio, cnt_sys);

        for (int i = 0; i < n; i++)
        {
            EXPECT_EQ(buf_rio[i], buf_sys[i]);
        }
    }

    close(fd_rio);
    close(fd_sys);
}

TEST(robustio_readnb, correctness_simple_cases)
{
    string path = "/home/fwei/learning/hydrogenWeb/tests/test_cases/text_simple.txt";

    int fd_rio = open(path.c_str(), O_RDONLY, 0);
    Riob riob(fd_rio);

    int fd_sys = open(path.c_str(), O_RDONLY, 0);
    ASSERT_TRUE(fd_sys >= 0);

    int count = 10;
    int bytes = sizeof(char);
    for (int n = 0; n < count; n++)
    {
        char buf_rio[RIO_BUFSIZE];
        char buf_sys[RIO_BUFSIZE];

        int cnt_rio = riob.readnb(buf_rio, n * bytes);
        int cnt_sys = read(fd_sys, buf_sys, n * bytes);
        EXPECT_EQ(cnt_rio, cnt_sys);

        for (int i = 0; i < n; i++)
        {
            EXPECT_EQ(buf_rio[i], buf_sys[i]);
        }
    }

    close(fd_rio);
    close(fd_sys);
}

TEST(robustio_readnb, correctness_large_cases)
{
    string path = "/home/fwei/learning/hydrogenWeb/tests/test_cases/text_large.txt";

    int fd_rio = open(path.c_str(), O_RDONLY, 0);
    Riob riob(fd_rio);

    int fd_sys = open(path.c_str(), O_RDONLY, 0);
    ASSERT_TRUE(fd_sys >= 0);

    int count = 10;
    int bytes = sizeof(char);
    for (int n = 0; n < count; n++)
    {
        char buf_rio[RIO_BUFSIZE];
        char buf_sys[RIO_BUFSIZE];

        int cnt_rio = riob.readnb(buf_rio, n * bytes);
        int cnt_sys = read(fd_sys, buf_sys, n * bytes);
        EXPECT_EQ(cnt_rio, cnt_sys);

        for (int i = 0; i < n; i++)
        {
            EXPECT_EQ(buf_rio[i], buf_sys[i]);
        }
    }

    close(fd_rio);
    close(fd_sys);
}

TEST(robustio_writen, correctness_simple_cases)
{
    string input_path = "/home/fwei/learning/hydrogenWeb/tests/test_cases/text_simple.txt";

    string rio_write_path = "/home/fwei/learning/hydrogenWeb/tests/test_cases/output/text_rio_write.txt";
    string sys_write_path = "/home/fwei/learning/hydrogenWeb/tests/test_cases/output/text_sys_write.txt";

    int fd_input = open(input_path.c_str(), O_RDONLY, 0);
    ASSERT_TRUE(fd_input >= 0);

    int count = 10;
    int bytes = sizeof(char);
    for (int n = 0; n < count; n++)
    {
        char buf_input[RIO_BUFSIZE];

        int cnt = Rio::readn(fd_input, buf_input, n * bytes);

        int fd_rio = open(rio_write_path.c_str(), O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
        int fd_sys = open(sys_write_path.c_str(), O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
        ASSERT_TRUE(fd_rio >= 0 && fd_sys >= 0);

        int cnt_rio = Rio::writen(fd_rio, buf_input, n * bytes);
        int cnt_sys = write(fd_sys, buf_input, n * bytes);
        EXPECT_TRUE(cnt == cnt_sys && cnt_rio == cnt_sys);

        char buf_in_rio[RIO_BUFSIZE];
        char buf_in_sys[RIO_BUFSIZE];
        int cnt_rd_rio = Rio::readn(fd_rio, buf_in_rio, n * bytes);
        int cnt_rd_sys = Rio::readn(fd_sys, buf_in_sys, n * bytes);
        EXPECT_EQ(cnt_rd_rio, cnt_rd_sys);

        for (int i = 0; i < n; i++)
        {
            EXPECT_EQ(buf_in_rio[i], buf_in_sys[i]);
        }

        close(fd_rio);
        close(fd_sys);
    }

    close(fd_input);
}

TEST(robustio_writen, correctness_large_cases)
{
    string input_path = "/home/fwei/learning/hydrogenWeb/tests/test_cases/text_large.txt";

    string rio_write_path = "/home/fwei/learning/hydrogenWeb/tests/test_cases/output/text_rio_write.txt";
    string sys_write_path = "/home/fwei/learning/hydrogenWeb/tests/test_cases/output/text_sys_write.txt";

    int fd_input = open(input_path.c_str(), O_RDONLY, 0);
    ASSERT_TRUE(fd_input >= 0);

    int count = 500;
    int bytes = sizeof(char);
    for (int n = 0; n < count; n++)
    {
        char buf_input[RIO_BUFSIZE];

        int cnt = Rio::readn(fd_input, buf_input, n * bytes);

        int fd_rio = open(rio_write_path.c_str(), O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
        int fd_sys = open(sys_write_path.c_str(), O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
        ASSERT_TRUE(fd_rio >= 0 && fd_sys >= 0);

        int cnt_rio = Rio::writen(fd_rio, buf_input, n * bytes);
        int cnt_sys = write(fd_sys, buf_input, n * bytes);
        EXPECT_TRUE(cnt == cnt_sys && cnt_rio == cnt_sys);

        char buf_in_rio[RIO_BUFSIZE];
        char buf_in_sys[RIO_BUFSIZE];
        int cnt_rd_rio = Rio::readn(fd_rio, buf_in_rio, n * bytes);
        int cnt_rd_sys = Rio::readn(fd_sys, buf_in_sys, n * bytes);
        EXPECT_EQ(cnt_rd_rio, cnt_rd_sys);

        for (int i = 0; i < n; i++)
        {
            EXPECT_EQ(buf_in_rio[i], buf_in_sys[i]);
        }

        close(fd_rio);
        close(fd_sys);
    }

    close(fd_input);
}
