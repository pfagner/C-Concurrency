#pragma once
#include <queue>
#include <condition_variable>

template <class T>
class queue
{
public:
    queue(const queue& other) = delete;
    queue& operator = (const queue& other) = delete;

    void push(const T& val)
    {
        std::lock_guard<std::mutex> l(mut);
        container.push(val);
        condVar.notify_one();
    }

    bool try_pop(T& val)
    {
        std::lock_guard<std::mutex> l(mut);
        if(!container.empty())
        {
            val = container.front();
            container.pop();
            return true;
        }
        return false;
    }

    void wait_and_pop(T& val)
    {
        std::unique_lock<std::mutex> l(mut);
        condVar.wait(l, [this](){ return !container.empty(); });
        val = container.front();
        container.pop();
    }

    bool empty()
    {
        std::lock_guard<std::mutex> l(mut);
        return container.empty();
    }

private:
    mutable std::mutex mut;
    std::queue<T> container;
    std::condition_variable condVar;
};
