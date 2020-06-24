/*
 * File: sem.hpp
 * ---------------------------------
 * @author: Fu Wei
 * An implementation of semaphore.
 * Refer to and inspired by:
 * - glibc/sem_waitcommon
 * - glibc/sem_wait
 * - glibc/sem_post
 * - Computer Systems: A Programmer's Perspective, pages about semaphore
 * - https://stackoverflow.com/questions/4792449/c0x-has-no-semaphores-how-to-synchronize-threads
 */

#ifndef HYDROGENWEB_SEM_HPP
#define HYDROGENWEB_SEM_HPP

#include <mutex>
#include <condition_variable>

class Sem
{
public:
    explicit Sem(int count);

    inline void wait()
    {
        std::unique_lock<std::mutex> lock(_mutex);

        while (_count == 0)
        {
            _cond.wait(lock);
        }
        _count--;
    }

    inline void post()
    {
        std::unique_lock<std::mutex> lock(_mutex);

        _count++;
        _cond.notify_one();
    }

private:
    int _count;
    std::mutex _mutex;
    std::condition_variable _cond;
};

#endif //HYDROGENWEB_SEM_HPP
