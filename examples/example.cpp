/*
 * @Author: Zhou Zijian 
 * @Date: 2023-04-21 02:33:32 
 * @Last Modified by:   Zhou Zijian 
 * @Last Modified time: 2023-04-21 02:33:32 
 */

#include <iostream>
#include <thread>
#include "BlockingQueue.h"

int main()
{
    BlockingQueue<int> queue;
    std::thread producer([&]() {
        for (int i = 0; i < 10; i++) {
            std::cout << "Produced: " << i << std::endl;
            queue.Push(i);
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    });
    std::thread consumer([&]() {
        for (int i = 0; i < 10; i++) {
            int value;
            queue.Pop(value, 2000);
            std::cout << "Consumed: " << value << std::endl;
        }
    });
    producer.join();
    consumer.join();
    std::cout << "Finished" << std::endl;
}
