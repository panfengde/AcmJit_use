/********************************************************************************
* @author: Mr PanFeng
* @email: 595464742@qq.com
* @date: 2024/12/15 15:30
* @description:
********************************************************************************/
#include <iostream>
#include <ostream>
#include <stdexcept>
#include <asmjit/x86.h>
#include <stdio.h>
using namespace asmjit;
using namespace std;

// Signature of the generated function.
typedef int (*SumFunc)(const int *arr, size_t count);

int main1() {
    JitRuntime rt; // Create a runtime specialized for JIT.
    CodeHolder code; // Create a CodeHolder.

    code.init(rt.environment(), // Initialize code to match the JIT environment.
              rt.cpuFeatures());
    x86::Assembler a(&code); // Create and attach x86::Assembler to code.

    // Decide between 32-bit CDECL, WIN64, and SysV64 calling conventions:
    //   32-BIT - passed all arguments by stack.
    //   WIN64  - passes first 4 arguments by RCX, RDX, R8, and R9.
    //   UNIX64 - passes first 6 arguments by RDI, RSI, RDX, RCX, R8, and R9.
    x86::Gp arr, cnt;
    x86::Gp sum = x86::eax; // Use EAX as 'sum' as it's a return register.

    if (ASMJIT_ARCH_BITS == 64) {
#if defined(_WIN32)
        arr = x86::rcx; // First argument (array ptr).
        cnt = x86::rdx; // Second argument (number of elements)
#else
     arr = x86::rdi;                 // First argument (array ptr).
     cnt = x86::rsi;                 // Second argument (number of elements)
#endif
    } else {
        arr = x86::edx; // Use EDX to hold the array pointer.
        cnt = x86::ecx; // Use ECX to hold the counter.
        // Fetch first and second arguments from [ESP + 4] and [ESP + 8].
        a.mov(arr, x86::ptr(x86::esp, 4));
        a.mov(cnt, x86::ptr(x86::esp, 8));
    }

    Label Loop = a.newLabel(); // To construct the loop, we need some labels.
    Label Exit = a.newLabel();

    //异或操作，自身进行亦或操作，总是为0，常用来清空
    a.xor_(sum, sum); // Clear 'sum' register (shorter than 'mov').
    // cnt & cnt，即测试 cnt 是否为 0。它不会修改 cnt，只更新标志寄存器的零标志 ZF。
    a.test(cnt, cnt); // Border case:
    // 根据零标志 ZF 来判断，如果 cnt 为 0，零标志就会被置为 1，跳转到 Exit。
    a.jz(Exit); //   If 'cnt' is zero jump to 'Exit' now.
    a.bind(Loop); // Start of a loop iteration.
    a.add(sum, x86::dword_ptr(arr)); // Add int at [arr] to 'sum'.
    a.add(arr, 4); // Increment 'arr' pointer.
    a.dec(cnt); // Decrease 'cnt'.
    a.jnz(Loop); // If not zero jump to 'Loop'.

    a.bind(Exit); // Exit to handle the border case.
    a.ret(); // Return from function ('sum' == 'eax').
    // ----> x86::Assembler is no longer needed from here and can be destroyed <----

    SumFunc fn;
    Error err = rt.add(&fn, &code); // Add the generated code to the runtime.

    if (err) return 1; // Handle a possible error returned by AsmJit.
    // ----> CodeHolder is no longer needed from here and can be destroyed <----

    static const int array[6] = {4, 8, 15, 16, 23, 42};

    int result = fn(array, 6); // Execute the generated code.
    printf("%d\n", result); // Print sum of array (108).

    rt.release(fn); // Explicitly remove the function from the runtime
    return 0; // Everything successful...
}

namespace learn1 {
    using sumFun = int(*)(int *, size_t count);
    //typedef int (*sumFun)(const int *arr, size_t count);

    sumFun generateSum() {
        auto rt = new JitRuntime();
        auto code = new CodeHolder();
        code->init(rt->environment(), rt->cpuFeatures());
        auto a = new x86::Assembler(code);

        x86::Gp arg1_arr, arg2_count;
        x86::Gp result = x86::eax; //eax是默认的返回寄存器
        if (ASMJIT_ARCH_BITS == 64) {
#if defined(_WIN32)
            // RCX,RDX,R8,R9
            arg1_arr = x86::rcx;
            arg2_count = x86::rdx;
#else
            arg1_arr = x86::rdi;                 // First argument (array ptr).
            arg2_count = x86::rsi;                 // Second argument (number of elements)
#endif
        } else {
            arg1_arr = x86::edx; // Use EDX to hold the array pointer.
            arg2_count = x86::ecx; // Use ECX to hold the counter.
            // Fetch first and second arguments from [ESP + 4] and [ESP + 8].
            a->mov(arg1_arr, x86::ptr(x86::esp, 4));
            a->mov(arg2_count, x86::ptr(x86::esp, 8));
        }

        Label Loop = a->newLabel();
        Label Exit = a->newLabel();

        a->xor_(result, result); //用异或操作清空
        //a->test(arg2_count, arg2_count); //按为与操作，只会修改零标志ZF，不修改arg2_count占用的寄存器
        a->cmp(arg2_count, 0);
        a->jz(Exit); //==0,跳转到Exit
        a->bind(Loop); //绑定锚点
        a->add(result, x86::dword_ptr(arg1_arr)); //从arg1_arr取4个字节，并加到result上面
        a->add(arg1_arr, 4); //arg1_arr地址偏移四个字节。因为int是4字节的
        a->dec(arg2_count);
        a->jnz(Loop); //!=0,跳转到Loop

        a->bind(Exit);
        a->ret(); //返回eax寄存器，约定eax是返回值（没有返回值就不用eax）

        sumFun fn = nullptr;
        Error err = rt->add(&fn, code);
        if (err)throw std::runtime_error("JIT Error");


        return fn;
    }
}

namespace learn2 {
    class Car {
    public:
        Car() { number = 10; }
        int number;
    };

    void changeType(Car *aCar, int aNumbernewName) {
        size_t offset = offsetof(Car, number);
        char *ptr = reinterpret_cast<char *>(aCar);
        *(reinterpret_cast<int *>(ptr + offset)) = 20;
    }

    void asmChangeName(Car *aCar, int newName) {
        auto offset = offsetof(Car, number);
        auto ptr = reinterpret_cast<uintptr_t>(aCar);
        auto valuePtr = ptr + offset;

        //int value = 0;
        JitRuntime rt;
        CodeHolder code;
        code.init(rt.environment(), rt.cpuFeatures());
        x86::Assembler a(&code);

        // 将 newName 加载到 rcx 寄存器
        a.mov(x86::rcx, newName);
        // 将 rcx 的值存储到 value 的地址
        a.mov(x86::ptr(uint64_t(valuePtr)), x86::rcx);
        // 函数返回，不返回任何值
        a.ret();

        // 编译生成的函数
        void (*func)() = nullptr;
        Error err = rt.add(&func, &code);
        if (err) {
            std::cerr << "asmChangeName error: " << err << std::endl;
            return;
        }

        // 执行汇编代码
        func();
        std::cout << "asmChangeName ok." << aCar->number << std::endl;
    }

    void test() {
        auto aCar = new Car();
        //changeType(aCar, 20);
        asmChangeName(aCar, 120);
        std::cout << "asmChangeName ok." << aCar->number << std::endl;
        //size_t offset = offsetof(Car, name);
    }
}

namespace learn3 {
    enum someType {
        one = 10,
        two
    };

    class Base {
    public:
        int theType = 10;
        char *baseName = "base";

        virtual void speak() {
            cout << "Base speak" << endl;
        }
    };

    class DataCar : public Base {
    public:
        DataCar() {
            name = new char[strlen("a car")]; // 动态分配内存
            strcpy(name, "a car"); // 将字符串拷贝到动态分配的内存中
            age = 123;
        }

        char *name;
        int age;

        void go() {
            cout << "DataCar speak" << endl;
        }

        void speak() override {
            cout << "DataCar speak" << endl;
        }
    };

    class DataAirPlane : public Base {
    public:
        DataAirPlane() {
            name = new char[strlen("airPlane")]; // 动态分配内存
            strcpy(name, "airPlane"); // 将字符串拷贝到动态分配的内存中
            age = 100;
        }

        char *name;
        int age;
    };

    size_t getStringDataOffset(std::string &str) {
        // 获取 std::string 对象本身的地址
        char *strPtr = reinterpret_cast<char *>(&str);

        // 获取 std::string 内部存储的字符数据的地址
        const char *dataPtr = str.data();

        // 计算对象和数据之间的偏移量
        return static_cast<size_t>(dataPtr - strPtr);
    }

    void useByPtr() {
        auto aCar = new DataCar();
        auto aFly = new DataAirPlane();


        auto typeOffset = offsetof(DataCar, theType);
        auto ageOffset = offsetof(DataCar, age);
        auto nameOffset = offsetof(DataCar, name);
        cout << *reinterpret_cast<int *>(reinterpret_cast<char *>(aCar) + typeOffset) << endl;
        cout << *reinterpret_cast<int *>(reinterpret_cast<char *>(aCar) + ageOffset) << endl;
        cout << *reinterpret_cast<char **>(reinterpret_cast<char *>(aCar) + nameOffset) << endl;


        ////// 调用虚函数
        // 获取虚函数表的地址
        void **vtable = *reinterpret_cast<void ***>(aCar);
        // 获取第一个虚函数地址，这里假设 speak 是第一个虚函数
        void (*speakFunc)() = reinterpret_cast<void(*)()>(vtable[0]);
        speakFunc(); // 输出: DataCar speak


        JitRuntime rt;
        CodeHolder code;
        code.init(rt.environment(), rt.cpuFeatures());
        x86::Assembler a(&code);
    }

    void setObjOffest(x86::Assembler *a, uintptr_t ptr, uintptr_t offset) {
        a->mov(x86::rax, uintptr_t(ptr));
        a->mov(x86::rcx, offset);
        a->add(x86::rax, x86::rcx);
    }

    //设置int指针的value
    void setInt(x86::Assembler *a, uintptr_t ptr, int value) {
        a->mov(x86::rcx, value);
        a->mov(x86::ptr(ptr), x86::rcx);
    }

    void useByASM() {
        auto aCar = new DataCar();
        auto aFly = new DataAirPlane();


        auto typeOffset = offsetof(DataCar, theType);
        auto ageOffset = offsetof(DataCar, age);
        auto nameOffset = offsetof(DataCar, name);
        cout << *reinterpret_cast<int *>(reinterpret_cast<char *>(aCar) + typeOffset) << endl;
        cout << *reinterpret_cast<int *>(reinterpret_cast<char *>(aCar) + ageOffset) << endl;
        cout << *reinterpret_cast<char **>(reinterpret_cast<char *>(aCar) + nameOffset) << endl;

        // 创建一个自定义的 Logger


        JitRuntime rt;
        CodeHolder code;
        code.init(rt.environment(), rt.cpuFeatures());
        x86::Assembler a(&code);

        a.mov(x86::rax, uintptr_t(aCar));
        a.mov(x86::rcx, ageOffset);
        a.add(x86::rax, x86::rcx);
        a.mov(x86::rcx, 20);
        a.mov(x86::ptr(x86::rax), x86::rcx);
        a.ret();

        // 输出格式化字符串
        //logger.logf("This is a formatted log: value = %d", 42);
        // 编译生成的函数
        void (*func)() = nullptr;
        Error err = rt.add(&func, &code);
        if (err) {
            std::cerr << "asmChangeName error: " << err << std::endl;
            return;
        }

        // 执行汇编代码
        func();
        std::cout << "aCar->age:" << aCar->age << std::endl;
    }

    void main() {
        useByASM();
    }
}

int main() {
    // auto fn = learn1::generateSum();
    // learn1::generateSum();
    // learn2::test();
    learn3::main();

    return 1;
}
