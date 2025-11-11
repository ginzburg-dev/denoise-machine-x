// ThreadPool.hpp
#pragma once

#include <dmxdenoiser/Logger.hpp>

#include <algorithm>
#include <condition_variable>
#include <cstddef>
#include <functional>
#include <future>
#include <iostream>
#include <memory>
#include <mutex>
#include <string>
#include <string_view>
#include <queue>
#include <type_traits> // for std::invoke_result_t
#include <thread>
#include <vector>
#include <unordered_map>

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

        static unsigned int maxThreads();
        std::size_t runningThreads() const;

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

    inline unsigned int ThreadPool::maxThreads() {
        int hc = std::thread::hardware_concurrency();
        return (hc > 1) ? hc - 1 : 1;
    }

    inline std::size_t ThreadPool::runningThreads() const {
        return m_workers.size();
    }

    inline ThreadPool::ThreadPool(int threads)
    {
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
        DMX_LOG_DEBUG("ThreadPool", "ThreadPool ", this, " created with ", threads, " threads.");
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
                DMX_LOG_ERROR("ThreadPool", "enqueue(): Enqueue on stopped ThreadPool ", this, " .");
                throw std::runtime_error("enqueue(): Enqueue on stopped ThreadPool.");
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
        
        DMX_LOG_TRACE("ThreadPool", "ThreadPool ", this, " stopped.");
    }

} // namespace dmxdenoiser
