# TripleItemPool(AsyncItemPool) 用于多线程的数据池

## 面向问题

在通信任务中存在两个线程，分别承担两个角色：发送者和接收者。  
当发送者的频率远远大于接收者的频率时，采用队列会产生数据堆积问题，并且在某些时刻需要清空队列。    
而接收者需要尽可能新的数据，并且不阻塞自己的工作线程。  

## 解决方案

在当前上下文中，管理一个包含三个物品的数组，并为物品上锁，附加原子状态：空、忙、满。  
发送者写入数据到数组中，寻找没有上锁，即没有设置成忙状态的物品，写入后将其设置为满状态。  
接收者读取数据，寻找满状态的物品，读取后将其设置为空状态。

在写入过程中，发送者在接收者不读取的情况下，也能继续写入，由于数组只保存三个数据，故不会产生数据堆积问题。
在读取过程中，接收者只有在有数据的情况下才会读取，不会阻塞发送者的写入。

## 使用示例

```c++
#include <iostream>
#include <chrono>
#include <threads>
#include <Cango/CommonUtils/AsyncItemPool.hpp>

void TestPool() {
    struct Data {
        int Key;
        double Value;
    };

    TripleItemPool<Data> data_pool;

    using namespace std::chrono_literals;

    std::thread sender{
        [&data_pool]() {
            std::this_thread::sleep_for(1s); // wait for receiver
            for (int i = 0; i < 100; ++i) {
                Data data{i, i * 1.1};
                data_pool.Write(data);
            }
        }
    };
    
    std::thread receiver{
        [&data_pool]() {
            for (int i = 0; i < 100; ++i) {
                Data data;
                data_pool.Read(data);
                std::cout << "Read: " << data.Key << " " << data.Value << std::endl;
            }
        }
    };

    sender.join();
    receiver.join();
}
```
