/*
 * File: shareBuff.cpp
 * -------------------
 * @author: Fu Wei
 * Implementation of sharedBuff package.
 */

#include "sem/sharedBuf.hpp"
#include <iostream>

SharedBuf::SharedBuf(int n) :
    slots(n), items(0), _capacity(n)
{
}

void SharedBuf::insert(int item)
{
    slots.wait();
    std::unique_lock<std::mutex> lock(_mutex);
    buf.push_back(item);
    lock.unlock();
    items.post();
}

int SharedBuf::remove()
{
    items.wait();
    std::unique_lock<std::mutex> lock(_mutex);
    int item = buf.front();
    buf.pop_front();
    lock.unlock();
    slots.post();
    return item;
}
