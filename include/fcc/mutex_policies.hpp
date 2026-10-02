#pragma once

#include <shared_mutex>

namespace fcc {

struct dummy_mutex {
    void lock() noexcept {}
    void unlock() noexcept {}
    void lock_shared() noexcept {}
    void unlock_shared() noexcept {}
};

using shared_mutex = std::shared_mutex;

struct stm32_interrupt_mutex {
public:
    void lock() noexcept {
        #if defined(__arm__) || defined(__thumb__)
        uint32_t current_primask = __get_PRIMASK();
        __disable_irq();

        if (m_nest_count == 0) {
            m_primask = current_primask;
        }
        m_nest_count++;
        #endif
    }
    
    void unlock() noexcept {
        #if defined(__arm__) || defined(__thumb__)
        if (m_nest_count > 0) {
            m_nest_count--;
            if (m_nest_count == 0) {
               __set_PRIMASK(m_primask);
            }
        }
        #endif
    }

    void lock_shared() noexcept { lock(); }
    void unlock_shared() noexcept { unlock(); }

private:
    uint32_t m_primask {0};
    uint32_t m_nest_count {0};
};

}