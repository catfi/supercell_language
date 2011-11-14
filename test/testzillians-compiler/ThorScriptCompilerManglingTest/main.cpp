#include <iostream>

#define ZERO 0

// no-param vs params vs return-value
void _func_00_() { }
void _func_01_(char x, char y) { }
//char _func_02_() { }

// primitive-type: signed vs unsigned
void _func_10_(char x) { }
void _func_11_(short x) { }
void _func_12_(int x) { }
void _func_13_(long x) { }
void _func_14_(long long x) { }
void _func_15_(unsigned char x) { }
void _func_16_(unsigned short x) { }
void _func_17_(unsigned int x) { }
void _func_18_(unsigned long x) { }
void _func_19_(unsigned long long x) { }

// non-primitive-type
struct QWE { };
void _func_20_(QWE x) { }
//QWE _func_21_(QWE x) { }

// pointer vs reference
void _func_30_(void* x) { }
void _func_31_(char& x) { }
void _func_32_(void** x) { }
void _func_33_(void*& x) { }
void _func_34_(void**& x) { }
void _func_35_(void (*_func_)(char x)) { }

// const vs non-const
void _func_40_(void const* x) { }
void _func_41_(void* const x) { }
void _func_42_(void const** x) { }
void _func_43_(void* const* x) { }
void _func_44_(void** const x) { }

// member-function: static vs non-static vs virtual
class ASD
{
public:
    void _func_45_() { }
    static void _func_46_() { }
    virtual void _func_47_() { }
    void _func_48_() const { }
    //const void* _func_49_() const { }
};

// namespace function
namespace ZXC
{
    void _func_50_() { }
}

int main()
{
    // no-param vs params vs return-value
    _func_00_();
    _func_01_(ZERO, ZERO);
    //_func_02_();

    // primitive-type: signed vs unsigned
    _func_10_(ZERO);
    _func_11_(ZERO);
    _func_12_(ZERO);
    _func_13_(ZERO);
    _func_14_(ZERO);
    _func_15_(ZERO);
    _func_16_(ZERO);
    _func_17_(ZERO);
    _func_18_(ZERO);
    _func_19_(ZERO);

    // non-primitive-type
    QWE qwe;
    _func_20_(qwe);
    //_func_21_(qwe);

    // pointer vs reference
    char x;
    void* y;
    void** z;
    _func_30_(ZERO);
    _func_31_(x);
    _func_32_(ZERO);
    _func_33_(y);
    _func_34_(z);
    _func_35_(ZERO);

    // const vs non-const
    _func_40_(ZERO);
    _func_41_(ZERO);
    _func_42_(ZERO);
    _func_43_(ZERO);
    _func_44_(ZERO);

    // member-function: static vs non-static vs virtual
    ASD asd;
    asd._func_45_();
    ASD::_func_46_();
    asd._func_47_();
    asd._func_48_();
}
