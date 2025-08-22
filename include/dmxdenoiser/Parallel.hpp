// Parallel.hpp
#pragma once

#include <dmxdenoiser/ThreadPool.hpp>

#include <algorithm>
#include <atomic>
#include <cstdint>
#include <cstddef>
#include <exception>
#include <functional>
#include <future>
#include <mutex>
#include <type_traits>
#include <vector> 

namespace dmxdenoiser 
{

    template<class F, 
        std::enable_if_t<std::is_invocable_v<F&, std::int64_t, std::int64_t>, int> = 0 >
    void parallelFor(std::int64_t start, std::int64_t end, 
                    F&& func, ThreadPool* pool = nullptr, std::int64_t chunkSize = 0) 
    {
        DMX_LOG_TRACE("Parallel", "parallelFor(int64_t, int64_t) invoked with parameters: start = ", 
            start, ", end = ",  end, ", threadPool = ", (pool == nullptr) ? "none" : "provided", ", chunkSize = ", chunkSize);
        std::atomic<bool> errorOccured{false};
        std::exception_ptr firstException = nullptr;
        std::mutex exceptMutex;

        const std::int64_t n = end - start;
        if (n <= 0) return;

        if (!pool) // If there's no ThreadPool, run on single thread
        {
            DMX_LOG_DEBUG("Parallel", "parallelFor(int64_t, int64_t): no ThreadPool avilable; runing single-threaded");
            std::invoke(std::forward<F>(func), start, end);
            return;
        }

        const auto threads = std::max<std::size_t>(1, pool->runningThreads());
        DMX_LOG_TRACE("Parallel", "parallelFor(int64_t, int64_t): threads = ", threads);

        if (chunkSize <= 0) {
            chunkSize = std::max<std::int64_t>(1, n / (8 * static_cast<std::int64_t>(threads)));
            DMX_LOG_TRACE("Parallel", "parallelFor(int64_t, int64_t): new chunkSize = ", chunkSize);
        }

        std::vector<std::future<void>> futures;
        futures.reserve((n + chunkSize - 1) / chunkSize);

        for(std::int64_t s = start; s < end; s += chunkSize) {
            const std::int64_t e = std::min(s + chunkSize, end);
            futures.emplace_back(pool->enqueue( [&, f = std::forward<F>(func), s, e]() mutable { 
                try
                {
                    if (!errorOccured) // Skip work after first error
                        std::invoke(f, s, e);
                }
                catch(const std::exception& e)
                {
                    errorOccured = true;
                    std::unique_lock<std::mutex> lock(exceptMutex);
                    if (!firstException) {
                        firstException = std::current_exception();
                        DMX_LOG_ERROR("Parallel", "parallelFor(int64_t, int64_t): ", e.what());
                    }
                }
                catch(...)
                {
                    errorOccured = true;
                    std::unique_lock<std::mutex> lock(exceptMutex);
                    if (!firstException) {
                        firstException = std::current_exception();
                        DMX_LOG_ERROR("Parallel", "parallelFor(int64_t, int64_t): Unknown exception.");
                    }
                }
            }));
        }

        for (auto& x: futures) x.get();

        if (errorOccured && firstException) std::rethrow_exception(firstException);
    }

    template<class F, 
        std::enable_if_t<std::is_invocable_v<F&, std::int64_t>, int> = 0 >
    inline void parallelFor(std::int64_t start, std::int64_t end, 
                            F&& func, ThreadPool* pool = nullptr, std::int64_t chunkSize = 0) 
    {
        DMX_LOG_TRACE("Parallel", "parallelFor(int64_t) invoked with parameters: start = ", 
            start, ", end = ",  end, ", threadPool = ", (pool == nullptr) ? "none" : "provided", ", chunkSize = ", chunkSize);

        parallelFor(start, end, [f = std::forward<F>(func)](std::int64_t s, std::int64_t e) mutable {
            for(std::int64_t i = s; i < e; ++i ) { std::invoke(f, i); }
        }, pool, chunkSize);
    }

} // namespace dmxdenoiser
