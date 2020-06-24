/*
 * File: sharedBuff_unittest.cpp
 * ---------------------------------
 * @author: Fu Wei
 *
 */

#include "sem/sharedBuf.hpp"
#include "gtest/gtest.h"
#include <string>

using namespace std;

void* produce_thread(void* vargp);
void* consume_thread(void* vargp);

const int NAGENTS = 20;
const int NELEMENTS = 20;
SharedBuf buf(40);
int item_count[NAGENTS * NELEMENTS]; // elements will be 0s by default initialization

TEST(sharedBuf, simple_producer_consumer_model)
{
    pthread_t ptid[NAGENTS];
    pthread_t ctid[NAGENTS];
    // create producer and consumer threads
    for (int id = 0; id < NAGENTS; id++)
    {
        int* vargp = new int;
        *(int*)vargp = id;
        pthread_create(&ptid[id], nullptr, produce_thread, vargp);

        vargp = new int;
        *(int*)vargp = id;
        pthread_create(&ctid[id], nullptr, consume_thread, vargp);
    }

    // join threads
    for (int i = 0; i < NAGENTS; i++)
    {
        pthread_join(ptid[i], nullptr);
        pthread_join(ctid[i], nullptr);
    }

    // check item_count
    for (int i = 0; i < NAGENTS * NELEMENTS; i++)
    {
        EXPECT_EQ(item_count[i], 1);
    }
}

void* produce_thread(void* vargp)
{
    int id = *(int*)vargp;
    delete (int*)vargp;

    for (int item = id * NELEMENTS; item < (id + 1) * NELEMENTS; item++)
    {
        buf.insert(item);
        //        cout << "Producer " << id << " produced value " << item << endl;
    }

    return nullptr;
}

void* consume_thread(void* vargp)
{
    int id = *(int*)vargp;
    delete (int*)vargp;

    for (int i = 0; i < NELEMENTS; i++)
    {
        int item = buf.remove();
        item_count[item]++;
        //        cout << "Consumer " << id << " cousumed value " << item << endl;
    }

    return nullptr;
}
