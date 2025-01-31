#include "ThreadsafeQueue.h"

template <typename T>
ThreadsafeQueue<T>::ThreadsafeQueue(size_t size_limit)
    : size_limit(size_limit), m_termination(false)
{
}

template <typename T>
std::shared_ptr<T> ThreadsafeQueue<T>::wait_and_pop()
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

template <typename T>
bool ThreadsafeQueue<T>::wait_and_pop(T &&value)
{
    std::shared_ptr<T> res = wait_and_pop();
    if (res == nullptr)
        return false;
    value = std::move(res);
    return true;
}

template <typename T>
std::shared_ptr<T> ThreadsafeQueue<T>::try_pop()
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

template <typename T>
bool ThreadsafeQueue<T>::try_pop(T &&value)
{
    std::shared_ptr<T> res = try_pop();
    if (res == nullptr)
        return false;
    value = std::move(res);
    return true;
}

template <typename T>
void ThreadsafeQueue<T>::move_push(T &&new_value)
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

template <typename T>
void ThreadsafeQueue<T>::push(T new_value)
{
    move_push(new_value);
}

template <typename T>
bool ThreadsafeQueue<T>::empty()
{
    std::unique_lock<std::mutex> lk(mut);
    return data_queue.empty();
}

template <typename T>
size_t ThreadsafeQueue<T>::size()
{
    std::unique_lock<std::mutex> lk(mut);
    return data_queue.size();
}

template <typename T>
size_t ThreadsafeQueue<T>::dropped_count() const
{
    return m_dropped_count.load(std::memory_order_relaxed);
}

template <typename T>
void ThreadsafeQueue<T>::termination()
{
    std::unique_lock<std::mutex> lk(mut);
    m_termination.store(true, std::memory_order_release);
    data_cond.notify_all();
}

template <typename T>
bool ThreadsafeQueue<T>::is_termination() const
{
    return m_termination.load(std::memory_order_acquire);
}
