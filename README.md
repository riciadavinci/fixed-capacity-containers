# Fixed Capacity Containers

<!-- Badges & Shields -->
[![C++20](https://img.shields.io/badge/C%2B%2B-20-blue.svg)](https://en.cppreference.com/w/cpp/20)
[![CMake](https://img.shields.io/badge/CMake-3.14%2B-064F8C.svg?logo=cmake)](https://cmake.org/)
[![Build Status](https://github.com/riciadavinci/fixed-capacity-containers/actions/workflows/ci.yaml/badge.svg)](https://github.com/riciadavinci/fixed-capacity-containers/actions)
[![codecov](https://codecov.io/gh/riciadavinci/fixed-capacity-containers/graph/badge.svg?token=YOUR_TOKEN)](https://codecov.io/gh/riciadavinci/fixed-capacity-containers)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)

## 1. Overview

**Fixed Capacity Containers** is a collection of data structures implemented in C++ (C++20 Standard) for use in my projects. It serves as a centralized place to store my custom implementations. 

All of these are header-only implementations. Because they use a nested directory structure to prevent naming conflicts, you simply add the `include/` directory to your project's include path and include the files. Example:

```cpp
#include <fcc/ring_buffer.hpp>
```

These containers are ideal for platforms where dynamic memory management (`new`/`delete`/`malloc`/`free`) is restricted or unavailable (such as Bare-Metal Embedded Systems), or where heap allocation is possible but undesirable due to fragmentation and latency constraints. 

On desktop and server targets, the containers also work inside `std::unique_ptr`, `std::shared_ptr`, and STL containers (like `std::unordered_map`, `std::map`, etc.).

<br>

<p align="right"><sub>[&nbsp;<a href="#fixed-capacity-containers">Back to Top</a> &nbsp;•&nbsp; <a href="#table-of-contents">Table of Contents</a>&nbsp;]</sub></p>

## Table of Contents
| # | Section |
| --- | --- |
| **1.** | [Overview](#1-overview) |
| **2.** | [Tech-Stack](#2-tech-stack) |
| **3.** | [Project Structure](#3-project-structure) |
| **4.** | [Installation & Integration](#4-installation--integration) |
|  | **4.1** &emsp;[Manual Include Path](#41-manual-include-path) |
|  | **4.2** &emsp;[Using _CMake_'s `FetchContent`](#42-using-cmakes-fetchcontent) |
| **5.** | [Containers & APIs](#5-containers--apis) |
|  | **5.1** &emsp;[Container: `ring_buffer`](#51-container-ring_buffer) |
|  | **5.2** &emsp;[Container: `queue`](#52-container-queue) |
|  | **5.3** &emsp;[Container: `lockfree_queue`](#53-container-lockfree_queue) |
| **6.** | [Mutex Policies](#6-mutex-policies) |
|  | **6.1** &emsp;[Mutex: `dummy_mutex`](#61-mutex-dummy_mutex) |
|  | **6.2** &emsp;[Mutex: `shared_mutex`](#62-mutex-shared_mutex) |
|  | **6.3** &emsp;[Mutex: `stm32_interrupt_mutex`](#63-mutex-stm32_interrupt_mutex) |
|  | **6.4** &emsp;[Example: Swapping Mutex Policies](#64-example-swapping-mutex-policies) |
| **7.** | [Memory Management and Safety Guards](#7-memory-management-and-safety-guards) |
|  | **7.1** &emsp;[Static or Global Allocation (Linker-Verified)](#71-static-or-global-allocation-linker-verified) |
|  | **7.2** &emsp;[Stack Allocation (Compile-Time Guarded via `static_assert`)](#72-stack-allocation-compile-time-guarded-via-static_assert) |
|  | **7.3** &emsp;[Heap Allocation (Desktop Lifetimes via Smart Pointers)](#73-heap-allocation-desktop-lifetimes-via-smart-pointers) |
|  | **7.4** &emsp;[Trivial Copyability Enforcement](#74-trivial-copyability-enforcement) |

<br>

<p align="right"><sub>[&nbsp;<a href="#fixed-capacity-containers">Back to Top</a> &nbsp;•&nbsp; <a href="#table-of-contents">Table of Contents</a>&nbsp;]</sub></p>


## 2. Tech-Stack

- _**Programming Language:** C++ (C++20 standard), C++ Standard Library_
- _**Testing & Code Coverage:** Google Test, Google Mock_
- _**Linting & Sanitizers:** Clang-Tidy, ASan, UBSan, TSan_
- _**Build Tools:** CMake 3.30.5, Ninja 1.31.1_
- _**Target OS Tested on:**  Windows 10 & Ubuntu 26.04_
- _**Compilers Tested on:**_
    - _**Windows:** MSVC 2022 (v19.41.34123 x64)_
    - _**Linux:** GCC (13.2) & Clang_ 

<br>

<p align="right"><sub>[&nbsp;<a href="#fixed-capacity-containers">Back to Top</a> &nbsp;•&nbsp; <a href="#table-of-contents">Table of Contents</a>&nbsp;]</sub></p>

## 3. Project Structure

```text
fixed-capacity-containers/
├── include/
│   └── fcc/
│       ├── mutex_policies.hpp
│       ├── ring_buffer.hpp
│       ├── queue.hpp
│       └── lockfree_queue.hpp
├── tests/                     
│   ├── CMakeLists.txt
│   ├── test_lockfree_queue.cpp
│   ├── test_queue.cpp
│   └── test_ring_buffer.cpp
├── LICENSE
├── README.md
└── CMakeLists.txt
```

<br>

<p align="right"><sub>[&nbsp;<a href="#fixed-capacity-containers">Back to Top</a> &nbsp;•&nbsp; <a href="#table-of-contents">Table of Contents</a>&nbsp;]</sub></p>

## 4. Installation & Integration

### Pre-requisites
Before building, ensure you have the following installed:
- _**CMake:** (v3.14 or higher)_
- _**C++20 Compiler:** (MSVC 2022, GCC 13+, or Clang 16+)_
- _**Ninja:** (Recommended build generator)_
- _**Git:** (Recommended for downloading)_

<br>

### 4.1 Manual Include Path

If you prefer not to use automated package management or automatic fetching utilities, `fixed-capacity-containers` can be cleanly integrated into any compilation workflow as a traditional header-only asset. Because all implementation details are contained within the template header files under the `include/` directory, no separate binary pre-compilation step is required.

#### 4.1.1 Step 1: Download the Source

Clone the repository or download the specific release archive directly into your project's directory structure (for example, placing it into a local `third_party/`, `libs/` or `deps/` folder):

```bash
git clone https://github.com/riciadavinci/fixed-capacity-containers.git third_party/fixed-capacity-containers
```

#### 4.1.2 Step 2: Update Your Build Configuration

To make the library discoverable to your compiler, you must add the library's `include/` path to your build tool's header search paths. This allows you to safely resolve standard application includes like `#include <fcc/ring_buffer.hpp>`.

Select the integration mechanism that matches your development environment:

##### Option A: Raw Compiler CLI (GCC / Clang)

When invoking the compiler directly via the Command Line Interface (CLI), append the include directory path using the standard `-I` search flag:

```bash
g++ -std=c++20 main.cpp -Ithird_party/fixed-capacity-containers/include -o my_application
```

##### Option B: Traditional GNU Makefiles

If you are managing your build process via a custom Makefile, append the path directly to your `CXXFLAGS` environment variable:

```makefile
# Define include directories
INCLUDE_FLAGS = -Ithird_party/fixed-capacity-containers/include

# Append to C++ compilation flags
CXXFLAGS += -std=c++20 $(INCLUDE_FLAGS)

all:
	$(CXX) $(CXXFLAGS) main.cpp -o my_application
```

##### Option C: Manual Subdirectory CMake Configuration

If you have embedded the raw source folder into your repository tree and wish to link it manually without using `FetchContent`, configure it via standard directory additions in your local `CMakeLists.txt`:

```cmake
cmake_minimum_required(VERSION 3.14)
project(my_application)

# Define the local location of the manually downloaded library
add_subdirectory(third_party/fixed-capacity-containers)

# Link your application target to the library interface target
add_executable(my_application main.cpp)
target_link_libraries(my_application PRIVATE fcc::containers)
```

##### Option D: Embedded Embedded IDEs (Keil / IAR / STM32CubeIDE)

For bare-metal microcontroller workspaces utilizing non-CMake graphical IDEs:
1. Open your project's **Properties / Build Settings**.
2. Navigate to the **C/C++ Build ➔ Settings ➔ Tool Settings ➔ Compiler ➔ Include Paths** tab.
3. Click **Add...** and browse to select the `third_party/fixed-capacity-containers/include` path on your disk.
4. Apply changes and trigger a clean project rebuild.

<br>

<p align="right"><sub>[&nbsp;<a href="#fixed-capacity-containers">Back to Top</a> &nbsp;•&nbsp; <a href="#table-of-contents">Table of Contents</a>&nbsp;]</sub></p>


### 4.2 Using _CMake's_ `FetchContent`

For modern CMake projects, you can integrate this library directly into your build system without downloading it manually. Add the following to your `CMakeLists.txt`:

```cmake
include(FetchContent)

FetchContent_Declare(
    fixed_capacity_containers
    GIT_REPOSITORY https://github.com/riciadavinci/fixed-capacity-containers.git
    GIT_TAG        main # Replace with a specific tag/commit hash for stability
)

FetchContent_MakeAvailable(fixed_capacity_containers)
```

#### Linking to your target
Once fetched, link your application or executable to the interface library target:

```cmake
target_link_libraries(your_project_target PRIVATE fcc::containers)
```

<br>

<p align="right"><sub>[&nbsp;<a href="#fixed-capacity-containers">Back to Top</a> &nbsp;•&nbsp; <a href="#table-of-contents">Table of Contents</a>&nbsp;]</sub></p>

## 5. Containers & APIs

### 5.1 Container: `ring_buffer`
A fixed-size ring buffer that overwrites older values when it reaches its maximum capacity. Functionally, it operates as a **Fixed-History Rolling Window Ring Buffer**, making it ideal for tracking real-time sensor metrics or plotting telemetry historical windows.

#### 5.1.1 Class API

```c++
namespace fcc {
    template<typename Type, size_t Capacity, typename MutexPolicy = dummy_mutex>
    class ring_buffer {
        // Constructor:
        ring_buffer() = default;

        // Inserting Item (Overwrites oldest if full):
        void push(Type item);

        // Logical Data Retrieval (Relative to chronological order):        
        std::optional<Type> get(size_t index) const;

        // Physical Data Retrieval (Direct access to internal raw buffer index):
        std::optional<Type> get_at_physical_index(size_t index) const;

        // Retrieving 'Latest' & 'Oldest' items:
        std::optional<Type> get_latest() const;
        std::optional<Type> get_oldest() const;

        // Getters:
        size_t head() const;
        size_t tail() const;
        size_t size() const;
        constexpr size_t capacity() const;

        // Useful helpers:
        bool full() const;
        bool empty() const;
    }
}
```

#### 5.1.2 Usage Example

```c++
#include <iostream>
#include <optional>
#include <fcc/ring_buffer.hpp>

struct TelemetryData {
    double timestamp;
    float temperature;
};

auto main() -> int {
    // 1. Creating a Rolling Buffer for the last 3 elements:
    auto buffer = fcc::ring_buffer<TelemetryData, 3>();

    // 2. Pushing historical sensor updates:
    buffer.push({100.0, 22.5f});
    buffer.push({101.0, 23.0f});
    buffer.push({102.0, 24.1f}); // Buffer is now full

    // 3. This push will cleanly overwrite the oldest item ({100.0, 22.5f}):
    buffer.push({103.0, 25.5f});

    // 4. Getting the absolute latest entry:
    auto latest = buffer.get_latest();
    if (latest.has_value()) {
        std::cout << "Latest Temp: " << latest->temperature << " C\n";
    }

    // 5. Zero-Allocation Slicing via the standard API loop:
    std::cout << "Iterating through the rolling window:\n";
    for (size_t i = 0; i < buffer.size(); ++i) {
        auto data_opt = buffer.get(i);
        if (data_opt.has_value()) {
            std::cout << " - Index [" << i << "]: " << data_opt->temperature << " C\n";
        }
    }
}
```

Output:

```text


```

<br>

<p align="right"><sub>[&nbsp;<a href="#fixed-capacity-containers">Back to Top</a> &nbsp;•&nbsp; <a href="#table-of-contents">Table of Contents</a>&nbsp;]</sub></p>

### 5.2 Container: `queue`

#### 5.2.1 Class API

```c++
namespace fcc {
    template<typename Type, size_t Capacity, typename MutexPolicy = dummy_mutex>
    class queue {
        // Constructor:
        queue() = default;

        // Inserting Item:
        bool push(Type item);

        // Retrieving Item:
        std::optional<Type> pop();

        // Getters:
        size_t size() const;
        constexpr size_t capacity() const;

        // Clearing the whole queue:
        void clear();

        // Useful helpers:
        bool full() const;
        bool empty() const;
    }
}
```

#### 5.2.2 Usage

```c++
#include <iostream>
#include <optional>
#include <fcc/queue.hpp>

auto main() -> int {
    // 1. Creating an Instance:
    constexpr size_t MAX_QUEUE_SIZE = 100;
    auto queue = fcc::queue<int, MAX_QUEUE_SIZE>();
    
    // Checking if 'queue' is empty:
    auto is_queue_empty_str = queue.empty() ? "true" : "false";
    std::cout << "'queue' is empty: " << is_queue_empty_str << std::endl;

    // 2. Inserting elements:
    if (queue.push(0)) {
        std::cout << "Item inserted into 'queue' successfully" << std::endl;
    }

    // 3. Retrieving (popping) elements:
    auto pop_opt = queue.pop();
    if (pop_opt.has_value()) {
        std::cout << "Retrieved item  'queue': " << pop_opt.value() << std::emdl;
    }

    // Since queue is empty, filling it with items
    for(size_t idx = 0; idx < queue.capacity(); ++idx) {
        queue.push(idx);
    }

    // Checking if 'queue' is full:
    auto is_queue_full_str = queue.full() ? "true" : "false";
    std::cout << "'queue' is full: " << is_queue_full_str << std::endl;

    // Since queue is full, attempting to fill item:
    if (!queue.push(500)) {
        std::cout << "'queue' is full. Cannot insert any more items. Remove an item with 'pop()' method first.\n";
    }

    // 4. Clearing 'queue' before inserting items again:
    queue.clear();

    for (size_t idx = 1; idx < 21; ++idx) {
        queue.push(idx*idx);
    }

    // 5. Accessing current head, current size, and total capacity:
    std::cout << "Current size of 'queue': " << queue.size() << std::endl;
    std::cout << "Total capacity of 'queue': " << queue.capacity() << std::endl;

    return EXIT_SUCCESS;
}
```

Output:

```text


```

<br>

<p align="right"><sub>[&nbsp;<a href="#fixed-capacity-containers">Back to Top</a> &nbsp;•&nbsp; <a href="#table-of-contents">Table of Contents</a>&nbsp;]</sub></p>

### 5.3 Container: `lockfree_queue`

#### 5.3.1 Class API

```c++
namespace fcc {
    template<typename Type, size_t Capacity>
    class lockfree_queue {
    public:
        // Constructor:
        lockfree_queue() = default;

        // Inserting Item (Thread-Safe / Non-Blocking):
        bool push(Type item);

        // Retrieving Item (Thread-Safe / Non-Blocking):
        std::optional<Type> pop();

        // Getters:
        size_t size() const;
        constexpr size_t capacity() const;

        // Clearing the whole queue:
        void clear();

        // Useful helpers:
        bool full() const;
        bool empty() const;
    };
}
```

#### 5.3.2 Usage

```c++
#include <iostream>
#include <thread>
#include <vector>
#include <fcc/lockfree_queue.hpp>

auto main() -> int {
    // 1. Creating a Lock-Free Instance:
    constexpr size_t MAX_QUEUE_SIZE = 500;
    auto lf_queue = fcc::lockfree_queue<int, MAX_QUEUE_SIZE>();

    // 2. Multi-Threaded Producer/Consumer Example:
    std::thread producer([&]() {
        for (int i = 0; i < 100; ++i) {
            while (!lf_queue.push(i)) {
                // Retry if the queue is temporarily full
                std::this_thread::yield();
            }
        }
    });

    std::thread consumer([&]() {
        int count = 0;
        while (count < 100) {
            auto item = lf_queue.pop();
            if (item.has_value()) {
                std::cout << "Consumed: " << item.value() << "\n";
                count++;
            } else {
                // Yield if the queue is temporarily empty
                std::this_thread::yield();
            }
        }
    });

    producer.join();
    consumer.join();

    // 3. Verifying remaining capacities using the standard API getters:
    std::cout << "Final size of 'lockfree_queue': " << lf_queue.size() << std::endl;
    std::cout << "Total capacity of 'lockfree_queue': " << lf_queue.capacity() << std::endl;
}
```

<br>

<p align="right"><sub>[&nbsp;<a href="#fixed-capacity-containers">Back to Top</a> &nbsp;•&nbsp; <a href="#table-of-contents">Table of Contents</a>&nbsp;]</sub></p>

## 6. Mutex Policies

To maximize performance across different runtime environments, the sequential containers (`fcc::queue` and `fcc::ring_buffer`) use a policy-driven thread-safety model via the `MutexPolicy template` parameter.
All custom policies implement a uniform interface supporting exclusive (`lock()` / `unlock()`) and shared (`lock_shared()` / `unlock_shared()`) execution locks, making them fully compatible with standard RAII wrappers like `std::lock_guard`, `std::shared_lock`, and `std::unique_lock`.

### 6.1 Mutex: `dummy_mutex`
A zero-overhead, completely transparent policy where all lock operations are empty, `noexcept` stubs.

- _**Use Case:**_ Single-threaded execution loops, interrupt service routines (ISRs) where thread safety is guaranteed by hardware architecture, or contexts where the container is already isolated. It ensures you pay absolutely zero performance penalty for synchronization features you don't need.

### 6.2 Mutex: `shared_mutex`
A direct type alias for `std::shared_mutex`.

- _**Use Case:**_ Standard multi-threaded desktop applications where multiple reader threads need concurrent access to inspect the container via shared locks (`lock_shared`), while ensuring strict exclusive access during writes (`lock`).

### 6.3 Mutex: `stm32_interrupt_mutex`
A specialized synchronization policy designed for ARM Cortex-M microcontrollers that cleanly abstracts target architecture checks away from the user. Intended for STM32 boards with single-core CPUs. For example, STM32F411RE with its ARM Cortex-M4 CPU.

- _**Implementation:**_ The target condition checks `(#if defined(__arm__) || defined(__thumb__))` are completely encapsulated inside the policy's implementation files. Calling lock() safely calls standard global interrupt disables (`__disable_irq()`) on hardware targets and resolves to a safe no-op on desktop environments for seamless testing. The shared lock variants evaluate to no-op stubs.
- _**Use Case:**_ Preventing race conditions between critical application threads and asynchronous Interrupt Service Routines (ISRs) on bare-metal hardware.

### 6.4 Example: Swapping Mutex Policies

```c++
#include <fcc/queue.hpp>
#include <fcc/mutex_policies.hpp>

auto main() -> int {
    // 1. Single-threaded / No overhead (Default behavior)
    auto local_queue = fcc::queue<int, 32, fcc::dummy_mutex>();

    // 2. Multi-threaded desktop environment
    auto safe_queue = fcc::queue<int, 1024, fcc::shared_mutex>();

    // 3. Embedded context safely synchronized against incoming ISRs 
    //    (Compiles seamlessly across both PC and ARM toolchains)
    auto embedded_queue = fcc::queue<int, 64, fcc::stm32_interrupt_mutex>();

    return EXIT_SUCCESS;
}
```

<br>

<p align="right"><sub>[&nbsp;<a href="#fixed-capacity-containers">Back to Top</a> &nbsp;•&nbsp; <a href="#table-of-contents">Table of Contents</a>&nbsp;]</sub></p>

## 7. Memory Management and Safety Guards

Because `fixed-capacity-containers` enforce strict compile-time capacity boundaries and eliminate dynamic runtime memory allocation (`new`/`delete`/`malloc`/`free`), the total memory footprint of any container is completely determined at instantiation. 

Depending on your target architecture, operating system constraints, and real-time requirements, you should structure your container declarations using one of three primary allocation strategies:

### 7.1 Static or Global Allocation (Linker-Verified)
When targeting resource-constrained microcontrollers with tight hardware memory maps (such as a single-core STM32 or dual-core RP2350 platform), allocating containers globally or utilizing the `static` storage duration keyword is the safest and most deterministic design pattern.

* **Memory Sector:** The container is placed permanently inside the `.bss` (uninitialized data) or `.data` (initialized data) segments of the chip's internal SRAM.
* **Safety Mechanism:** The allocation boundary is verified entirely by the compiler toolchain's **Linker Script (`.ld`)** during the final compilation phase. If the container size exceeds the physical SRAM limits of your target silicon, the linker will immediately abort the build phase.
* **Developer Advantage:** You receive absolute compile-time safety without any runtime checking overhead. Your firmware is mathematically guaranteed to fit on the target hardware before it is ever flashed to the board.

```cpp
#include <fcc/ring_buffer.hpp>

// Allocated safely inside the static memory sector (.bss). 
// The linker will automatically fail the build if this overflows the physical SRAM.
static fcc::ring_buffer<TelemetryData, 1000> g_sensor_stream;
```

### 7.2 Stack Allocation (Compile-Time Guarded via `static_assert`)
Declaring a container locally inside a standard function scope or within a dedicated RTOS thread task routine places the entire structure directly onto the execution stack frame.

* **Memory Sector:** The active execution stack of the running thread or microcontroller core.
* **The Risk:** Thread and task stacks on embedded targets are heavily restricted (often limited to a few kilobytes or words). Allocating an excessively large container inside a local scope will trigger immediate, catastrophic stack overflows or memory corruption at runtime.
* **Safety Mechanism:** Because the exact size of an FCC container is known at compile-time via `sizeof(Container)`, you can use `static_assert` to prevent dangerous footprints before the binary is ever generated.
* **Developer Advantage:** Wrap local task instantiations in a protective compile-time assertion to ensure the allocation stays well below your thread or task group safety limits:

```cpp
void processing_task(void* pvParameters) {
    // Define the safe stack boundary for this specific execution context
    constexpr std::size_t MAX_SAFE_TASK_STACK_BYTES = 2048;
    using TargetBuffer = fcc::ring_buffer<TelemetryData, 50>;
    
    // Compile-time Guard: Fails compilation if the container breaches stack safety limits
    static_assert(sizeof(TargetBuffer) < MAX_SAFE_TASK_STACK_BYTES, 
                  "FCC Error: Local container allocation exceeds safe thread stack threshold!");
                  
    TargetBuffer local_stack_buffer; // Safe local stack allocation
}
```

### 7.3 Heap Allocation (Desktop Lifetimes via Smart Pointers)
When using the library inside host desktop environments or distributed high-performance backend servers, memory constraints are vastly relaxed, and heap allocation can be leveraged safely.

* **Memory Sector:** The system heap via modern C++ smart pointer wrappers like `std::unique_ptr` & `std::shared_ptr`, or use heap allocated containers whose memory is managed automatically like `std::map` or `std::unordered_map`. Example for smart pointers: 
```c++
// Heap Allocated Buffers (using unique_ptr):
auto unique_buffer = std::make_unique<fcc::ring_buffer<float, 1000>>();

// Heap Allocated Buffers (using shared_ptr):
auto shared_buffer = std::make_shared<fcc::ring_buffer<uint64_t, 2000>>();
```
* **Safety Mechanism:** Because modern operating systems (Windows, Linux, macOS) feature massive virtual memory subsystems backed by physical RAM and disk paging, a fixed-capacity container of typical application scale carries virtually zero risk of exhausting the system memory during initialization.
* **Developer Advantage:** No explicit compilation size guards are necessary. By wrapping your fixed-capacity containers inside smart pointers, you can dynamically control the exact *lifetime* of your rolling history windows while still benefiting from the library’s underlying data locality, fast array indexing speeds, and absolute lack of inner runtime allocations.

```cpp
#include <unordered_map>
#include <fcc/ring_buffer.hpp>

// Ensure the target structure type is visible to the scope
struct TelemetryData {
    float temperature;
    float pressure;
    float humidity;
};

class LiveTelemetryRepository {
private:
    // Safely allocated on the heap inside a desktop environment.
    // Preserves the cache-locality benefits of FCC with zero risk of stack overflow.
    std::unordered_map<std::string, fcc::ring_buffer<TelemetryData, 3600>> m_buffers;
public:
    LiveTelemetryRepository() = default;

    void insertData(const std::string& node_id, const TelemetryData& data) {
        // Perform a fast lookup without forcing a map-key string copy upfront
        auto it = m_buffers.find(node_id);

        if (it != m_buffers.end()) {
            // Core Path: Node exists! Push data into the buffer with zero runtime allocations.
            it->second.push(data);
        } else {
            // Exceptional Path: Brand new node. We explicitly insert a fresh buffer.
            auto [new_it, inserted] = m_buffers.emplace(node_id, fcc::ring_buffer<TelemetryData, 3600>{});
            if (inserted) {
                new_it->second.push(data);
            }
        }
    }
};
```
***

### 7.4 Trivial Copyability Enforcement
To guarantee that your fixed-capacity containers remain lightning-fast and safe for bare-metal systems, every container in the library enforces a compile-time trait check on its data type:

```cpp
static_assert(std::is_trivially_copyable_v<Type>, 
              "FCC Error: Elements stored in fixed-capacity containers must be trivially copyable!");
```

This guard ensures that users don't accidentally drop complex heap-allocated objects (like `std::string` or `std::vector`) into your embedded arrays. It guarantees that the container can optimize its internal operations using hyper-fast, low-level CPU byte-copying instructions, maintaining the absolute zero-allocation invariant of your library.

<br>

<p align="right"><sub>[&nbsp;<a href="#fixed-capacity-containers">Back to Top</a> &nbsp;•&nbsp; <a href="#table-of-contents">Table of Contents</a>&nbsp;]</sub></p>
