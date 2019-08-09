#pragma once

#include <atomic>
#include <thread>
#include <shared_mutex>

namespace flo {

    class RecursiveSharedMutex {
    public:
        RecursiveSharedMutex() noexcept;
        ~RecursiveSharedMutex() noexcept = default;

        void lock(){
            const auto thrid = std::this_thread::get_id();
            if (thrid == m_owner){
                ++m_count;
            } else {
                m_mutex.lock();
                m_owner = thrid;
                m_count = 1;
            }
        }

        void lock_shared(){
            const auto thrid = std::this_thread::get_id();
            if (thrid == m_owner){
                throw std::runtime_error("Don't do that.");
            }
            m_mutex.lock_shared();
        }

        void unlock(){
            if (m_count > 1){
                --m_count;
            } else {
                m_owner = std::thread::id{};
                m_count = 0;
                m_mutex.unlock();
            }
        }

        void unlock_shared(){
            m_mutex.unlock_shared();
        }

    private:
        std::shared_mutex m_mutex;
        std::atomic<std::thread::id> m_owner;
        unsigned m_count;
    };

} // namespace flo
