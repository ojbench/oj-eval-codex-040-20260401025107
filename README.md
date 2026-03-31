# Problem 040 - stargazer 的双链表

**ACMOJ Problem ID**: 2640

## Table of Contents

- [Problem 040 - stargazer 的双链表](#problem-040-stargazer-的双链表)
  - [Table of Contents](#table-of-contents)
  - [Introduction](#introduction)
    - [Background](#background)
  - [Assignment Description](#assignment-description)
    - [Grade Composition](#grade-composition)
  - [Assignment Requirements](#assignment-requirements)
    - [Input Format](#input-format)
    - [Output Format](#output-format)
    - [Samples](#samples)
    - [Data Constraints](#data-constraints)
  - [Per-Testcase Resource Limits](#per-testcase-resource-limits)
  - [Test Data](#test-data)
  - [Submission Requirements](#submission-requirements)
    - [OJ Git Repository Compilation Process](#oj-git-repository-compilation-process)
    - [Git Configuration Requirements](#git-configuration-requirements)
    - [Submission Guidelines](#submission-guidelines)
    - [Evaluation Notes](#evaluation-notes)
    - [Academic Integrity](#academic-integrity)

## Introduction

### Background

Implement doubly linked list using Rust-like Rc/Weak smart pointers in C++. Tests ownership semantics.

## Assignment Description

### Grade Composition

| Grading Component | Percentage |
| :--: | :--: |
| Pass **2640. stargazer 的双链表** | 80% |
| Code Review | 20% |

Here are several points that need clarification:

- In the Code Review, we will **strictly examine your code style and repository organization structure, etc.**

- This assignment provides some sample data for testing, stored in the `/workspace/data/040/data_test/` directory. Note that these are not the test cases on the Online Judge. Passing all local test cases does not guarantee that you will pass the OJ tests.

- Besides the provided sample data, we also encourage you to design your own test data based on your program logic to assist debugging.

## Assignment Requirements

### Problem Description

> "学完[Rust]对我写C++影响很大"
>
> ——Dark


stargazer 最近正在学习 Rust 语言的所有权机制与内存安全特性。

Rust 的所有权机制要求每个值都有唯一的所有者（通常是变量），并且在同一时间内只能有一个所有者。当变量的所有权被转移或其生命周期结束后，所有对该变量的借用（相当于 C++ 中指向变量的指针）都不能再被使用，否则会造成编译错误。Rust 的编译器可以在编译时就能“静态”地检查所有权和借用关系，从而在运行时无需额外检查的情况下保证了内存安全。

这种所有权机制虽然保证了内存安全，但是也使得一些数据结构的实现变得困难。因此，对于堆上对象的所有权，Rust 引入了类似于 C++ 中智能指针的机制，包括：
- `Rc<T>`：相当于 C++ 中的 `std::shared_ptr`，允许多个所有者共享实际对象，通过引用计数管理内存
- `Weak<T>`：相当于 C++ 中的 `std::weak_ptr`，不增加强引用计数，用于解决循环引用问题
- `Weak<T>` 必须先通过 `upgrade()` 方法转换为 `Rc<T>` 才能访问实际对象。若实际对象已被释放，则 `upgrade()` 会返回 `None`。C++ 中的 `std::weak_ptr` 也有类似的 `lock()` 方法。

在具体实现中：
- `Rc<T>` 与 `Weak<T>` 并不直接指向对象本身，而是指向一个“引用计数块”，内含三个成员：“强引用计数”、“弱引用计数”、“指向对象的指针”
- 强引用计数：指向该对象的 `Rc<T>` 的数量
- 弱引用计数：指向该对象的 `Weak<T>` 的数量
- 当强引用计数为 0 时，实际对象会被释放
- 当弱引用计数也为 0 时，引用计数块会被释放

`Weak<T>` 避免了循环引用问题。在双链表中，每个节点需要指向前一个和后一个节点。如果所有这些指针都是强引用（如 `Rc<T>` 或 `std::shared_ptr`），就会形成循环引用：节点A持有指向节点B的强引用，节点B也持有指向节点A的强引用，这会导致两者的引用计数永远不会降为0，即使外部不再引用这些节点，它们也不会被释放，从而造成内存泄漏。使用 `Weak<T>`（或 `std::weak_ptr`）可以打破这种循环。

请借鉴 Rust 中的所有权思想，在 C++ 中实现一个无需手动管理内存的双链表。

本题允许查看 cppreference 中以下内容：
- `std::shared_ptr` [中文](https://zh.cppreference.com/w/cpp/memory/shared_ptr) [英文](https://en.cppreference.com/w/cpp/memory/shared_ptr)
- `std::weak_ptr` [中文](https://zh.cppreference.com/w/cpp/memory/weak_ptr) [英文](https://en.cppreference.com/w/cpp/memory/weak_ptr)
- `std::optional` [中文](https://zh.cppreference.com/w/cpp/utility/optional) [英文](https://en.cppreference.com/w/cpp/utility/optional)

```cpp
#include <iostream>
#include <memory>
#include <optional>

template <typename T>
class DoublyLinkedList {
private:
    // Node structure definition
    struct Node {
        T value;
        std::shared_ptr<Node> next;
        std::weak_ptr<Node> prev;

        Node(const T& val) : value(val), next(nullptr) {}
    };

    std::shared_ptr<Node> head;
    std::weak_ptr<Node> tail;
    size_t length;

public:
    // Iterator class
    class iterator {
    private:
        std::shared_ptr<Node> current;

    public:
        iterator(std::shared_ptr<Node> node) : current(node) {}

        // Dereference operator to get value
        T& operator*() {
            // 注：对空迭代器调用此函数是未定义行为，测试数据中不会出现
            // TODO(student)
        }

        // Post-increment operator
        iterator operator++(int) {
            // 注：对空迭代器调用此函数是未定义行为，测试数据中不会出现
            // TODO(student)
        }

        // Equality operators
        bool operator==(const iterator& other) const {
            // 注：比较属于不同容器的非空迭代器是未定义行为，测试数据中不会出现
            // TODO(student)
        }

        bool operator!=(const iterator& other) const {
            // TODO(student)
        }
    };

    // Constructor
    DoublyLinkedList() : head(nullptr), length(0) {}

    // No destructor needed, memory managed by shared_ptr

    // Check if list is empty
    bool empty() const {
        return length == 0;
    }

    // Get the size of the list
    size_t size() const {
        return length;
    }

    // Add element to the front
    void push_front(const T& value) {
        // TODO(student)
    }

    // Add element to the back
    void push_back(const T& value) {
        // TODO(student)
    }

    // Remove and return the first element
    std::optional<T> pop_front() {
        if (!head) {
            return std::nullopt;
        }
        // TODO(student)
    }

    // Remove and return the last element
    std::optional<T> pop_back() {
        if (empty()) {
            return std::nullopt;
        }
        // TODO(student)
    }

    // Iterators
    iterator begin() {
        return iterator(head);
    }

    iterator end() {
        return iterator(nullptr);
    }
};
```

## 备注

- 不可以更改除了 `TODO(student)` 以外的任何代码。
- 不可以使用 `new` `delete` 等方法手动管理内存。

### Input Format

See the problem description above.

### Output Format

See the problem description above.

### Samples

No sample data provided for this problem.

### Data Constraints

- $30$ 分：实现 `push_front`, `pop_front` 方法。
- $60$ 分：实现 `push_front`, `pop_front` 与迭代器。
- $100$ 分：实现所有方法。

数据点由 `valgrind` 内存检查。

## Per-Testcase Resource Limits

- **Time Limit (per test case)**: 100000 ms
- **Memory Limit (per test case)**: 256 MiB
- **Disk Usage**: No disk usage is permitted.

## Test Data

The test data for this problem is located at `/workspace/data/040/data_test/`.

## Submission Requirements

### OJ Git Repository Compilation Process

For Git compilation, we will first clone the repository using a command similar to:
```bash
git clone <repo_url> . --depth 1 --recurse-submodules --shallow-submodules --no-local
```

Then we check if there is a `CMakeLists.txt` file. If it exists, we run (if not, a warning message will be displayed):
```bash
cmake .
```

Finally, we check if there is any of `GNUmakefile`/`makefile`/`Makefile` (if cmake was run previously, this will be the generated Makefile). If it exists, we run (if not, a warning message will be displayed):
```bash
make
```

After this process is complete, we will use the `code` file in the project root directory as the compilation result.

A `CMakeLists.txt` file is provided in the project. You can use or modify it as needed. The local environment has gcc-13 and g++-13 available.

### Git Configuration Requirements

**IMPORTANT**: You must create a `.gitignore` file in your project root directory to avoid OJ evaluation conflicts.

The `.gitignore` file should include at least the following entries:

```gitignore
CMakeFiles/
CMakeCache.txt
```

### Submission Guidelines

- The submitted code must be able to compile successfully through the above compilation process
- The compiled executable file name must be `code`
- The program needs to be able to read data from standard input and write results to standard output
- Please ensure the code runs correctly within the given time and space limits
- **You must use C++ or C language** to implement this assignment

### Evaluation Notes

- The evaluation system will test your program using the provided test data
- The program output must exactly match the expected output (including format)
- Exceeding time or memory limits will be judged as the corresponding error type
- Please pay attention to the overall time performance of your code and the time complexity of each part of your algorithm.

### Academic Integrity

If any violations are found during evaluation or code review (including but not limited to using unconventional methods to pass test cases), your final score may be significantly reduced or become **0 points**.
