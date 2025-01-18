/********************************************************************************
* @author: Mr PanFeng
* @email: 595464742@qq.com
* @date: 2024/12/15 15:30
* @description:
********************************************************************************/
#include <asmjit/asmjit.h>
#include <iostream>
#include <vector>
#include <chrono>

using namespace asmjit;

// 假设有三个复杂的函数：Foo, Bar 和 Baz
// 复杂版的 Foo 函数
void one() {
    volatile int dummy = 0;
    for (int i = 0; i < 1000000; ++i) {
        dummy += i;
    }
    if (dummy == -1) {
        std::cout << "Impossible" << std::endl;
    }
}

// 复杂版的 Bar 函数
void two() {
    volatile int dummy = 0; // 使用 volatile 防止编译器优化
    for (int i = 0; i < 1000000; ++i) {
        dummy += i;
    }
    if (dummy == -1) {
        // 加入一个不会执行的条件，避免未使用警告
        std::cout << "Impossible" << std::endl;
    }
}

// 复杂版的 Baz 函数
void three() {
    volatile int dummy = 0; // 使用 volatile 防止编译器优化
    for (int i = 0; i < 1000000; ++i) {
        dummy += i;
    }
    if (dummy == -1) {
        // 加入一个不会执行的条件，避免未使用警告
        std::cout << "Impossible" << std::endl;
    }
}

int numTests = 1000; // 测试的执行次数
int notJITTest() {
    // 使用 chrono 测量函数指针调用的时间
    auto start = std::chrono::high_resolution_clock::now();
    auto onePtr = &one;
    auto twoPtr = &two;
    auto threePtr = &three;
    for (int i = 0; i < numTests; ++i) {
        onePtr(); // 调用 Foo()
        twoPtr(); // 调用 Bar()
        threePtr(); // 调用 Foo()
        onePtr(); // 调用 Foo()
        twoPtr(); // 调用 Bar()
        threePtr(); // 调用 Foo()
        onePtr(); // 调用 Foo()
        twoPtr(); // 调用 Bar()
        threePtr(); // 调用 Foo()
        onePtr(); // 调用 Foo()
        twoPtr(); // 调用 Bar()
        threePtr(); // 调用 Foo()
        onePtr(); // 调用 Foo()
        twoPtr(); // 调用 Bar()
        threePtr(); // 调用 Foo()
        onePtr(); // 调用 Foo()
        twoPtr(); // 调用 Bar()
        threePtr(); // 调用 Foo()
        onePtr(); // 调用 Foo()
        twoPtr(); // 调用 Bar()
        threePtr(); // 调用 Foo()
        onePtr(); // 调用 Foo()
        twoPtr(); // 调用 Bar()
        threePtr(); // 调用 Foo()
    }

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = end - start;

    std::cout << "notJIT:Time taken for " << numTests << " iterations: " << duration.count() << " seconds." <<
            std::endl;

    return 0;
}

int JITTest() {
    std::cout << "Foo called\n";
    // 初始化 JIT 编译器
    JitRuntime rt;
    CodeHolder code;
    code.init(rt.environment());

    // 创建 x86 汇编生成器
    x86::Assembler a(&code);

    // 生成 Test 调用顺序：Foo, Bar, Foo, Baz, Bar, Foo, Baz
    a.call(reinterpret_cast<void *>(one)); // 调用 Foo()
    a.call(reinterpret_cast<void *>(two)); // 调用 Bar()
    a.call(reinterpret_cast<void *>(three)); // 调用 Foo()
    a.call(reinterpret_cast<void *>(one)); // 调用 Foo()
    a.call(reinterpret_cast<void *>(two)); // 调用 Bar()
    a.call(reinterpret_cast<void *>(three)); // 调用 Foo()
    a.call(reinterpret_cast<void *>(one)); // 调用 Foo()
    a.call(reinterpret_cast<void *>(two)); // 调用 Bar()
    a.call(reinterpret_cast<void *>(three)); // 调用 Foo()
    a.call(reinterpret_cast<void *>(one)); // 调用 Foo()
    a.call(reinterpret_cast<void *>(two)); // 调用 Bar()
    a.call(reinterpret_cast<void *>(three)); // 调用 Foo()
    a.call(reinterpret_cast<void *>(one)); // 调用 Foo()
    a.call(reinterpret_cast<void *>(two)); // 调用 Bar()
    a.call(reinterpret_cast<void *>(three)); // 调用 Foo()
    a.call(reinterpret_cast<void *>(one)); // 调用 Foo()
    a.call(reinterpret_cast<void *>(two)); // 调用 Bar()
    a.call(reinterpret_cast<void *>(three)); // 调用 Foo()
    a.call(reinterpret_cast<void *>(one)); // 调用 Foo()
    a.call(reinterpret_cast<void *>(two)); // 调用 Bar()
    a.call(reinterpret_cast<void *>(three)); // 调用 Foo()
    a.call(reinterpret_cast<void *>(one)); // 调用 Foo()
    a.call(reinterpret_cast<void *>(two)); // 调用 Bar()
    a.call(reinterpret_cast<void *>(three)); // 调用 Foo()
    a.ret();

    // 生成并执行机器码
    typedef void (*Fn)();
    Fn fn;
    Error err = rt.add(&fn, &code);
    if (err) {
        std::cerr << "Error generating code: " << err << std::endl;
        return -1;
    }

    auto start = std::chrono::high_resolution_clock::now();

    for (int i = 0; i < numTests; ++i) {
        fn(); // 执行生成的机器码
    }


    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = end - start;
    std::cout << "JIT:Time taken for  " << numTests << " iterations: " << duration.count() << " seconds." << std::endl;
    std::cout << "Successfully executed the generated machine code." << std::endl;

    return 0;
}

int main() {
    notJITTest();
    JITTest();
    return 0;
}


class aCarType {
public:
    aCarType(std::string aname) {
        name = aname;
    }

    void show() {
        std::cout << name << std::endl;
    }

    std::string name;
};

void test() {
    auto aCar = new aCarType("testCar");
    aCar->show();
}
