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
void Foo() {
    // 模拟一些复杂计算
    for ( int i = 0; i < 1000000; ++i) {
    }
}

// 复杂版的 Bar 函数
void Bar() {
    // 模拟一些复杂计算
    for ( int i = 0; i < 1000000; ++i) {
    }
}

// 复杂版的 Baz 函数
void Baz() {
    // 模拟一些复杂计算
    for ( int i = 0; i < 1000000; ++i) {
    }
}

int numTests = 1; // 测试的执行次数
int notJITTest() {
    // 使用 chrono 测量函数指针调用的时间
    auto start = std::chrono::high_resolution_clock::now();

    for (int i = 0; i < numTests; ++i) {
        Foo(); // 调用 Foo()
        Bar(); // 调用 Bar()
        Baz(); // 调用 Foo()
        Foo(); // 调用 Foo()
        Bar(); // 调用 Bar()
        Baz(); // 调用 Foo()
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
    a.call(reinterpret_cast<void *>(Foo)); // 调用 Foo()
    a.call(reinterpret_cast<void *>(Bar)); // 调用 Bar()
    a.call(reinterpret_cast<void *>(Baz)); // 调用 Foo()
    a.call(reinterpret_cast<void *>(Foo)); // 调用 Foo()
    a.call(reinterpret_cast<void *>(Bar)); // 调用 Bar()
    a.call(reinterpret_cast<void *>(Baz)); // 调用 Foo()

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

class Car{
  public:
    Car(std::string aname){
        name=aname;
    };
    void showName(){

    }
    std::string name;
};

int test(int type){
      if(type==1){
           auto aCar= new Car("hello");
          aCar->showName();
          return type;
      }else{
          auto aCar= new Car("hello");
          aCar->showName();
          return type;
      }
}