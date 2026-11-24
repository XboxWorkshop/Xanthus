#pragma once

// runs work posted from any thread on the main thread (input requests and config callbacks)
class MainThreadDispatcher
{
public:
    using Task = std::function<void()>;

    static MainThreadDispatcher& Get()
    {
        static MainThreadDispatcher instance;
        return instance;
    }

    void Post(Task task)
    {
        std::lock_guard lock(m_mutex);
        m_queue.push_back(std::move(task));
    }

    void Flush()
    {
        {
            std::lock_guard lock(m_mutex);
            std::swap(m_running, m_queue);
        }
        for (auto& task : m_running)
            task();

        m_running.clear();
    }

private:
    std::mutex m_mutex;
    std::vector<Task> m_queue;
    std::vector<Task> m_running;
};
