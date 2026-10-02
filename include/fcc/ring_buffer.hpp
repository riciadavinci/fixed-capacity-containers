#pragma once

#include <array>
#include <mutex>
#include <shared_mutex>
#include <optional>
#include "mutex_policies.hpp"

namespace fcc {

template<typename Type, size_t Capacity, typename MutexPolicy = dummy_mutex>
class ring_buffer {
    static_assert(Capacity > 0, "fcc::ring_buffer Capacity must be greater that 0");

public:
    // Constructor:
    ring_buffer() = default;

    // Inserting Item (Overwrites oldest if full):
    void push(Type item) {
        std::unique_lock lock(m_mutex);

        m_data[m_head] = item;
        m_head = (m_head + 1) % Capacity;

        if (m_current_size < Capacity) {
            m_current_size++;
        } else {
            m_tail = (m_tail + 1) % Capacity;
        }
    }

    // Logical Data Retrieval (Relative to chronological order):        
    [[nodiscard]] std::optional<Type> get(size_t index) const {
        std::shared_lock lock(m_mutex);
        
        if ((m_current_size == 0) || (index >= m_current_size)) {
            return std::nullopt;
        }

        size_t actual_intex = (m_tail + index) % Capacity;

        return m_data[actual_intex];
    }

    // Physical Data Retrieval (Direct access to internal raw buffer index):
    [[nodiscard]] std::optional<Type> get_at_physical_index(size_t index) const {
        std::shared_lock lock(m_mutex);

        if ((m_current_size == 0) || (index >= m_current_size)) {
            return std::nullopt;
        }

        return m_data[index];
    }

    // Retrieving 'Latest' & 'Oldest' items:
    [[nodiscard]] std::optional<Type> get_latest() const {
        std::shared_lock lock(m_mutex);

        if (m_current_size == 0) {
            return std::nullopt;
        }
        size_t index = (m_head + Capacity - 1 ) % Capacity;
        return m_data[index];
    }

    [[nodiscard]] std::optional<Type> get_oldest() const {
        std::shared_lock lock(m_mutex);

        if (m_current_size == 0) {
            return std::nullopt;
        }

        return m_data[m_tail];
    }

    // Getters:
    [[nodiscard]] size_t head() const {
        std::shared_lock lock(m_mutex);
        return m_head;
    }

    [[nodiscard]] size_t tail() const {
        std::shared_lock lock(m_mutex);
        return m_tail;
    }
    
    [[nodiscard]] size_t size() const {
        std::shared_lock lock(m_mutex);
        return m_current_size;
    }
    
    [[nodiscard]] constexpr size_t capacity() const {
        return Capacity;
    }

    // Useful helpers:
    [[nodiscard]] bool full() const {
        std::shared_lock lock(m_mutex);
        return (m_current_size == Capacity);
    }

    [[nodiscard]] bool empty() const {
        std::shared_lock lock(m_mutex);
        return (m_current_size == 0);
    }

    [[nodiscard]] constexpr size_t size_in_bytes() const {
        return sizeof(Type) * Capacity;
    }

private:
    std::array<Type, Capacity> m_data {};
    size_t m_head {0UL};
    size_t m_tail {0UL};
    size_t m_current_size {0UL};
    mutable MutexPolicy m_mutex;
};

}
