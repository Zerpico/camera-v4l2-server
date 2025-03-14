#pragma once

#include <queue>
#include <mutex>
#include <condition_variable>
#include <memory>
#include <atomic>

template <typename T>
class ThreadsafeQueue
{
public:
    ThreadsafeQueue(size_t size_limit = 10)
        : size_limit(size_limit), m_termination(false)
    {
    }
    ~ThreadsafeQueue() = default;

    // return nullptr if the queue is empty
    std::shared_ptr<T> wait_and_pop()
    {
        std::unique_lock<std::mutex> lk(mut);
        data_cond.wait(lk, [this]
                       { return !data_queue.empty() ||
                                m_termination.load(std::memory_order_acquire); });

        // dequeue if not empty
        if (!data_queue.empty())
        {
            std::shared_ptr<T> res = data_queue.front();
            data_queue.pop();
            return res;
        }

        // If the queue is empty, return nullptr
        return nullptr;
    }

    // return false if the queue is empty
    bool wait_and_pop(T &value)
    {
        std::shared_ptr<T> res = wait_and_pop();
        if (res == nullptr)
            return false;
        value = std::move(*res);
        return true;
    }

    // return nullptr if the queue is empty
    std::shared_ptr<T> try_pop()
    {
        std::lock_guard<std::mutex> lk(mut);

        // dequeue if not empty
        if (!data_queue.empty())
        {
            std::shared_ptr<T> res = data_queue.front();
            data_queue.pop();
            return res;
        }

        // If the queue is empty, return nullptr
        return nullptr;
    }

    // return false if the queue is empty
    bool try_pop(T &value)
    {
        std::shared_ptr<T> res = try_pop();
        if (res == nullptr)
            return false;
        value = std::move(*res);
        return true;
    }

    // insert queue
    void push(T new_value)
    {
        move_push(new_value);
    }

    bool empty()
    {
        std::unique_lock<std::mutex> lk(mut);
        return data_queue.empty();
    }

    size_t size() const
    {
        std::unique_lock<std::mutex> lk(mut);
        return data_queue.size();
    }

    size_t dropped_count() const
    {
        return m_dropped_count.load(std::memory_order_relaxed);
    }

    // Set this queue to terminated state.
    // In the exit state, the enqueue is ignored, and the dequeue can be performed.
    // When the queue is empty, wait_and_pop will not block.
    void termination()
    {
        std::unique_lock<std::mutex> lk(mut);
        m_termination.store(true, std::memory_order_release);
        data_cond.notify_all();
    }

    // Get whether this queue is terminated
    bool is_termination() const
    {
        return m_termination.load(std::memory_order_acquire);
    }

private:
    mutable std::mutex mut;
    std::queue<std::shared_ptr<T>> data_queue;
    const size_t size_limit;
    std::condition_variable data_cond;
    std::atomic_bool m_termination;
    std::atomic_size_t m_dropped_count;
    void move_push(T &new_value)
    {
        if (m_termination.load(std::memory_order_acquire))
            return;
        std::shared_ptr<T> data(std::make_shared<T>(std::move(new_value)));
        std::unique_lock<std::mutex> lk(mut);
        data_queue.push(data);
        if (data_queue.size() > size_limit)
        {
            data_queue.pop();
            m_dropped_count.fetch_add(1, std::memory_order_relaxed);
        }
        data_cond.notify_one();
    }
};
