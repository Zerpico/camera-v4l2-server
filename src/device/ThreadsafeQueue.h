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
    ThreadsafeQueue(size_t size_limit)
        : size_limit(size_limit), m_termination(false)
    {
    }
    ~ThreadsafeQueue() = default;

    /**
     * 1. When termination is not called, one element is dequeued every time the
     *    queue is called until the queue is empty. This method blocks the thread.
     * 2. After termination is called, this method will never block. If it is
     *    already in a blocked state, contact the blocked state.
     * 3. When true is returned, the value is valid. When false is returned, value
     *    is invalid. Returns false when termination is called and the queue is empty.
     **/

    // return nullptr if the queue is empty
    std::shared_ptr<T> wait_and_pop();

    // return false if the queue is empty
    bool wait_and_pop(T &&value);

    // return nullptr if the queue is empty
    std::shared_ptr<T> try_pop();

    // return false if the queue is empty
    bool try_pop(T &&value);

    // insert queue, move
    void move_push(T &&new_value);

    // insert queue
    void push(T new_value);

    bool empty();

    size_t size();

    size_t dropped_count() const;

    // Set this queue to terminated state.
    // In the exit state, the enqueue is ignored, and the dequeue can be performed.
    // When the queue is empty, wait_and_pop will not block.
    void termination();

    // Get whether this queue is terminated
    bool is_termination() const;

private:
    std::mutex mut;
    std::queue<std::shared_ptr<T>> data_queue;
    const size_t size_limit;
    std::condition_variable data_cond;
    std::atomic_bool m_termination;
    std::atomic_size_t m_dropped_count;
};