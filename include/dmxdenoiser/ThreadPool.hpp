// ThreadPool.hpp
//
// Note: This thread pool is intentionally **not implemented as a singleton**.
// It is designed so that multiple thread pools can exist simultaneously,
// allowing you to separate fast and slow tasks into different pools.
//
// This separation enables better resource allocation and can improve performance
// for fast tasks, as slow-running jobs cannot block the fast queue.

#pragma once

#include <dmxdenoiser/Logger.hpp>

#include <algorithm>
#include <condition_variable>
#include <functional>
#include <future>
#include <iostream>
#include <memory>
#include <mutex>
#include <queue>
#include <type_traits> // for std::invoke_result_t
#include <thread>
#include <vector>

namespace dmxdenoiser 
{

    class ThreadPool
    {
    public:
        ThreadPool(int threads);
        ~ThreadPool();
        ThreadPool(const ThreadPool&) = delete;
        ThreadPool& operator=(const ThreadPool&) = delete;
        ThreadPool(const ThreadPool&&) = delete;
        ThreadPool& operator=(const ThreadPool&&) = delete;

        static int maxThreads();
        int runningThreads() const;

        template<class F, class... Args>
        auto enqueue(F&& f, Args&&... args) 
            -> std::future<typename std::invoke_result_t<F, Args...>>;

    private:
        std::vector<std::thread> m_workers;
        std::queue<std::function<void()>> m_tasks;

        std::mutex m_mutex;
        std::condition_variable m_cv;
        bool m_stop{false};
    };

    int ThreadPool::maxThreads() {
        int hc = std::thread::hardware_concurrency();
        return (hc > 1) ? hc - 1 : 1;
    }

    inline int ThreadPool::runningThreads() const {
        return m_workers.size();
    }

    inline ThreadPool::ThreadPool(int threads) {
        threads = threads > 0 ? 
            ( threads <= this->maxThreads() ? threads : this->maxThreads() ) : this->maxThreads();

        for(int i = 0; i < threads; ++i)
        {
            m_workers.emplace_back(
                [this](){
                    for(;;)
                    {
                        std::function<void()> task;

                        {
                            std::unique_lock<std::mutex> lock(m_mutex);
                            m_cv.wait(lock, [this](){ return m_stop || !m_tasks.empty(); });
                            if ( m_stop && m_tasks.empty() )
                                return;
                            task = std::move(m_tasks.front());
                            m_tasks.pop();
                        }
                        task();
                    }
                }
            );
        }
    }

    template<class F, class... Args>
    auto ThreadPool::enqueue(F&& f, Args&&... args) 
        -> std::future<std::invoke_result_t<F, Args...>> 
    {
        using returnType =  std::invoke_result_t<F, Args...>;
        auto task = std::make_shared<std::packaged_task<returnType()>>(
            std::bind(std::forward<F>(f), std::forward<Args>(args)...));
        auto result = task->get_future();
        {
            std::unique_lock<std::mutex> lock(m_mutex);
            if (m_stop)
            {
                DMX_LOG_ERROR("ThreadPool::enqueue()", "Enqueue on stopped ThreadPool.");
                throw std::runtime_error("Enqueue on stopped ThreadPool.");
            }
            m_tasks.emplace([task](){ (*task)(); });
        }
        m_cv.notify_one();
        return result;
    }

    inline ThreadPool::~ThreadPool() 
    {
        {
            std::unique_lock<std::mutex> lock(m_mutex);
            m_stop = true;
        }
        m_cv.notify_all();
        for(std::thread& worker : m_workers)
            worker.join();
    }

} // namespace dmxdenoiser
