#include <asmjit/asmjit.h>
#include <asmjit/a64.h>
#include <iostream>

using namespace asmjit;

int main()
{
    // 创建 JitRuntime 对象
    JitRuntime rt;

    // 创建代码容器（CodeHolder）来保存生成的代码
    CodeHolder code;
    code.init(rt.environment(), rt.cpuFeatures());

    // 创建 ARM64 汇编器 (使用 a64 命名空间)
    a64::Assembler a(&code);

    a.mov(a64::x0, imm(10));
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
