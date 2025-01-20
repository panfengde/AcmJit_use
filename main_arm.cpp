#include <asmjit/asmjit.h>
#include <asmjit/a64.h>
#include <iostream>
#include <string>
#include <cstddef>
using namespace asmjit;
using namespace std;


namespace learn1
{
    class Car
    {
    public:
        Car() { number = 10; }
        int number;
    };

    void changeType(Car* aCar, int aNumbernewName)
    {
        size_t offset = offsetof(Car, number);
        char* ptr = reinterpret_cast<char*>(aCar);
        *(reinterpret_cast<int*>(ptr + offset)) = 20;

    }

    void asmChangeName(Car* aCar, int newName)
    {
        auto offset = offsetof(Car, number);
        auto ptr = reinterpret_cast<uintptr_t>(aCar);
        //auto valuePtr = ptr + offset;
        auto xx=a64::Mem(arm::ptr(valuePtr));
        int value=0;
        auto valuePtr=reinterpret_cast<uintptr_t>(&value);
        JitRuntime rt;
        CodeHolder code;
        code.init(rt.environment(), rt.cpuFeatures());
        a64::Assembler a(&code);

        a.mov(a64::x0, newName);
        a.str(a64::x0, a64::Mem(arm::ptr(valuePtr)));
        a.ret(a64::x30); //什么都不返回

        void (*func)() = nullptr;
        Error err = rt.add(&func, &code);
        if (err)
        {
            std::cerr << "asmChangeName error: " << err << std::endl;
            return;
        }

        // 执行生成的汇编代码
        func();
        std::cerr << "asmChangeName error: " << err << std::endl;

        std::cout << "asmChangeName ok."  << std::endl;
        return;
    }

    void test()
    {
        auto aCar = new Car();
        //changeType(aCar, 20);
        asmChangeName(aCar, 20);
        cout << "number:" << aCar->number << endl;
        //size_t offset = offsetof(Car, name);
    }

    int main()
    {
        // 创建 JitRuntime 对象
        JitRuntime rt;
        // 创建代码容器（CodeHolder）来保存生成的代码
        CodeHolder code;
        code.init(rt.environment(), rt.cpuFeatures());

        // 创建 ARM64 汇编器 (使用 a64 命名空间)
        a64::Assembler a(&code);

        a.mov(a64::x0, 10);
        a.mov(a64::x1, 20);
        a.add(a64::x0, a64::x0, a64::x1); // x0 = x0 + x1 (即 10 + 20)
        a.ret(a64::x30); //什么都不返回
        // a64::x0存储要返回的值
        // 调用函数后必须使用a.ret(a64::x30);返回原来的地址并跳转

        int (*func)() = nullptr;
        Error err = rt.add(&func, &code);
        if (err)
        {
            std::cerr << "Error: " << err << std::endl;
            return -1;
        }

        // 执行生成的汇编代码
        auto result = func();
        std::cout << "Result is stored in x0 (this would be done via additional code)." << result << std::endl;
        return 0;
    }
}

int main()
{
    learn1::test();
}

int main2()
{
    // 创建 JitRuntime 对象
    JitRuntime rt;

    // 创建代码容器（CodeHolder）来保存生成的代码
    CodeHolder code;
    code.init(rt.environment(), rt.cpuFeatures());

    // 创建 ARM64 汇编器 (使用 a64 命名空间)
    a64::Assembler a(&code);

    a.mov(a64::x0, 10);
    a.mov(a64::x1, 20);
    a.add(a64::x0, a64::x0, a64::x1); // x0 = x0 + x1 (即 10 + 20)
    a.ret(a64::x30); //什么都不返回
    // a64::x0存储要返回的值
    // 调用函数后必须使用a.ret(a64::x30);返回原来的地址并跳转

    int (*func)() = nullptr;
    Error err = rt.add(&func, &code);
    if (err)
    {
        std::cerr << "Error: " << err << std::endl;
        return -1;
    }

    // 执行生成的汇编代码
    auto result = func();
    std::cout << "Result is stored in x0 (this would be done via additional code)." << result << std::endl;
    return 0;
}
