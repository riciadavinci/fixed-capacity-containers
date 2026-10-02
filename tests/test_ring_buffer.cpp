#include <gtest/gtest.h>
#include <fcc/mutex_policies.hpp>
#include <fcc/ring_buffer.hpp>

#include <optional>
#include <thread>
#include <cstdint>

namespace fcc::testing {

// For RingBufferEdgeCasesTest_EmbeddedStructSupport Test Case:
struct TelemetryData {
    uint32_t timestamp;
    float temperature;
    uint16_t adc_raw;

    bool operator==(const TelemetryData&) const = default;
};

class RingBufferTest : public ::testing::Test {
protected:
    static constexpr std::size_t TEST_CAPACITY = 5;
    fcc::ring_buffer<int, TEST_CAPACITY, fcc::dummy_mutex> buffer;
};

TEST(RingBufferCompileTimeTest, ConstexprGuarantees) {
    using TestBuffer = fcc::ring_buffer<uint32_t, 10, fcc::dummy_mutex>;
    constexpr TestBuffer buf{};

    static_assert(buf.capacity() == 10UL, "Capacity must be evaluated at compile time");
    static_assert(buf.size_in_bytes() == 40UL, "Byte size calculation is incorrect");

    EXPECT_EQ(buf.capacity(), 10UL);
    EXPECT_EQ(buf.size_in_bytes(), 40UL);
}

TEST_F(RingBufferTest, InitiallyEmpty) {
    EXPECT_TRUE(buffer.empty());
    EXPECT_FALSE(buffer.full());
    EXPECT_EQ(buffer.size(), 0UL);
    EXPECT_EQ(buffer.head(), 0UL);
    EXPECT_EQ(buffer.tail(), 0UL);

    EXPECT_EQ(buffer.get_oldest(), std::nullopt);
    EXPECT_EQ(buffer.get_latest(), std::nullopt);
    EXPECT_EQ(buffer.get(0), std::nullopt);
    EXPECT_EQ(buffer.get_at_physical_index(0), std::nullopt);
}

TEST_F(RingBufferTest, PushingSingleItem) {
    buffer.push(100);

    EXPECT_FALSE(buffer.empty());
    EXPECT_FALSE(buffer.full());
    EXPECT_EQ(buffer.size(), 1UL);
    EXPECT_EQ(buffer.get_latest(), 100);
    EXPECT_EQ(buffer.get_oldest(), 100);
    EXPECT_EQ(buffer.get(0), 100);
    EXPECT_EQ(buffer.get_at_physical_index(0), 100);
}

TEST_F(RingBufferTest, SequentialLogicalIndexing) {
    buffer.push(10);
    buffer.push(20);
    buffer.push(30);

    EXPECT_EQ(buffer.size(), 3UL);
    EXPECT_EQ(buffer.get(0), 10);
    EXPECT_EQ(buffer.get(1), 20);
    EXPECT_EQ(buffer.get(2), 30);
    EXPECT_EQ(buffer.get(3), std::nullopt);
}

TEST_F(RingBufferTest, FillToCapacity) {
    for (size_t i = 1; i <= 5; ++i) {
        buffer.push(static_cast<int>(i)*10);
    }

    EXPECT_TRUE(buffer.full());
    EXPECT_FALSE(buffer.empty());
    EXPECT_EQ(buffer.get_oldest(), 10);
    EXPECT_EQ(buffer.get_latest(), 50);
}

// Circular Overwrite Logic Test

TEST_F(RingBufferTest, OverWriteOldestWhenFull) {
    for (size_t i = 1; i <= 5; ++i) {
        buffer.push(static_cast<int>(i)*10);
    }

    buffer.push(60);

    EXPECT_TRUE(buffer.full());
    EXPECT_EQ(buffer.size(), 5UL);
    EXPECT_EQ(buffer.get_oldest(), 20); // Previous value 10 was overwritten
    EXPECT_EQ(buffer.get_latest(), 60);

    // Verify updated logical order
    EXPECT_EQ(buffer.get(0), 20);
    EXPECT_EQ(buffer.get(1), 30);
    EXPECT_EQ(buffer.get(2), 40);
    EXPECT_EQ(buffer.get(3), 50);
    EXPECT_EQ(buffer.get(4), 60);
}


TEST_F(RingBufferTest, MultipleWrapAroundCycles) {
    for (size_t i = 1; i <= 25; ++i) {
        buffer.push(static_cast<int>(i));
    }

    EXPECT_TRUE(buffer.full());
    EXPECT_EQ(buffer.size(), 5UL);
    EXPECT_EQ(buffer.get_latest(), 25);
    EXPECT_EQ(buffer.get_oldest(), 21);
}


TEST_F(RingBufferTest, PhysicalIndexAccess) {
    for (size_t i = 1; i <=7; ++i) {
        buffer.push(static_cast<int>(i*10));
    }

    EXPECT_EQ(buffer.get_at_physical_index(0), 60);
    EXPECT_EQ(buffer.get_at_physical_index(1), 70);

    EXPECT_EQ(buffer.get_at_physical_index(5), std::nullopt);
    EXPECT_EQ(buffer.get_at_physical_index(100), std::nullopt);
}



// Edge Cases:

TEST(RingBufferEdgeCasesTest, CapacityOneBuffer) {
    fcc::ring_buffer<int, 1, fcc::dummy_mutex> single_buf;

    EXPECT_TRUE(single_buf.empty());
    single_buf.push(67);
    EXPECT_TRUE(single_buf.full());
    EXPECT_EQ(single_buf.get_latest(), 67);

    single_buf.push(100);
    EXPECT_TRUE(single_buf.full());
    EXPECT_EQ(single_buf.get_latest(), 100);
}


// Embedded Systems Data Struct Edge Test Case:
// Checking if non-primitives work
TEST(RingBufferEdgeCasesTest, EmbeddedStructSupport) {
    fcc::ring_buffer<TelemetryData, 3, fcc::dummy_mutex> sensor_buffer;

    TelemetryData td1{1000, 24.6F, 0x01FF};
    TelemetryData td2{1001, 26.1F, 0x0200};

    sensor_buffer.push(td1);
    sensor_buffer.push(td2);

    EXPECT_EQ(sensor_buffer.get(0), td1);
    EXPECT_EQ(sensor_buffer.get(1)->temperature, 26.1F);
}


// Multi-Threaded Concurrency Test (Desktop/Server), ThreadSanitizer Target

TEST(RingBufferConcurrencyTest, ConcurrencyPushAndGet) {
    fcc::ring_buffer<int, 64, fcc::shared_mutex> mt_buffer;
    std::atomic<bool> running {true};
    std::atomic<size_t> successful_reads {0};

    std::thread producer([&]() {
        for (size_t i = 0; i < 5000; ++i) {
            mt_buffer.push(static_cast<int>(i));
        }
        running = false;
    });

    std::thread consumer([&]() {
        while (running) {
            if (auto val = mt_buffer.get_latest()) {
                (void) val;
                successful_reads.fetch_add(1, std::memory_order_relaxed);
            } else {
                std::this_thread::yield();
            }
        }
    });


    producer.join();
    consumer.join();

    EXPECT_FALSE(mt_buffer.empty());
    EXPECT_GT(successful_reads.load(), 0UL);
}

} // namespace fcc::testing