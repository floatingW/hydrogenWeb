/*
 * File: sharedBuff.hpp
 * --------------------
 * @author: Fu Wei
 * A buff that can synchronize concurrent accesses.
 * Inspired by SBUF package from Computer Systems: A Programmer's Perspective.
 * // TODO: template
 */

#ifndef HYDROGENWEB_SHAREDBUF_HPP
#define HYDROGENWEB_SHAREDBUF_HPP

#include "sem/sem.hpp"
#include <deque>

class SharedBuf
{
public:
    explicit SharedBuf(int n);

    void insert(int item);
    int remove();

private:
    std::deque<int> buf;
    int _capacity;
    std::mutex _mutex;
    Sem slots;
    Sem items;
};
#endif //HYDROGENWEB_SHAREDBUF_HPP
