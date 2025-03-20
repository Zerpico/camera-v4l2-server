#pragma once

#include <functional>
#include <memory>
#include <queue>
#include <mutex>
#include <thread>
#include <condition_variable>

/// @brief Асинхронная очередь задач
class AsyncTaskQueue
{
public:
    using Task = std::function<void()>;

    AsyncTaskQueue() : isRunning(true), workerThread(&AsyncTaskQueue::worker, this) {}

    ~AsyncTaskQueue()
    {
        {
            std::unique_lock<std::mutex> lock(queueMutex);
            isRunning = false;
        }
        condition.notify_one();
        workerThread.join();
    }

    /// @brief Добавить задачу в очередь на выполнение
    /// @param task std::function<void()>
    void enqueue(Task task)
    {
        {
            std::unique_lock<std::mutex> lock(queueMutex);
            taskQueue.push(std::move(task));
        }
        condition.notify_one();
    }

private:
    void worker()
    {
        while (isRunning)
        {
            Task task;
            {
                std::unique_lock<std::mutex> lock(queueMutex);
                condition.wait(lock, [this]
                               { return !taskQueue.empty() || !isRunning; });

                if (!taskQueue.empty())
                {
                    task = std::move(taskQueue.front());
                    taskQueue.pop();
                }
                else if (!isRunning)
                {
                    break;
                }
                else
                {
                    continue;
                }
            }
            task();
        }
    }

    std::queue<Task> taskQueue;
    std::mutex queueMutex;
    std::condition_variable condition;
    std::atomic<bool> isRunning;
    std::thread workerThread;
};