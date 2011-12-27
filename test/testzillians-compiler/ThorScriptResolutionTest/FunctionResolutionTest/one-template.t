//////////////////////////////////////////////////////////////////////////////
// some classes
//////////////////////////////////////////////////////////////////////////////

    class Foo {}
    class Bar {}
    class Base {}
    class Derived extends Base {}
    class Extended extends Derived {}
    class Complex<T> {}

//////////////////////////////////////////////////////////////////////////////
// 1 template functions
//////////////////////////////////////////////////////////////////////////////

    // full generalized
    @static_test { resolution="f<T>(T)"                               } function f<T>(a:T) : void {}
    @static_test { resolution="f<T>(T,T)"                             } function f<T>(a:T, b:T) : void {}
    @static_test { resolution="f<T>(T,T,T)"                           } function f<T>(a:T, b:T, c:T) : void {}
    // specilaized
    @static_test { resolution="f<int8>(T)"                            } function f<T:int8>(a:T) : void {}
    @static_test { resolution="f<Foo>(T)"                             } function f<T:Foo>(a:T) : void {}
    @static_test { resolution="f<Base>(T)"                            } function f<T:Base>(a:T) : void {}
    @static_test { resolution="f<Complex<int32>>(T)"                  } function f<T:Complex<T:int32> >(a:T) : void {}
    @static_test { resolution="f<Complex<int64>>(T)"                  } function f<T:Complex<T:int64> >(a:T) : void {}
    // non-template
    @static_test { resolution="f(int32)"                              } function f(a:int32) : void {}

    // g for and not deduceded
    @static_test { resolution="g<T>(T,T)"                             } function g<T>(a:T, b:T) : void {}
    @static_test { resolution="g<int32>(int32,int32)"                 } function g<T:int32>(a:T, b:T) : void {}
    @static_test { resolution="g<Foo>(Foo,Foo)"                       } function g<T:Foo>(a:T, b:T) : void {}

    // no-parameter template function
    @static_test { resolution="h<T>()"                                } function h<T>() : void {}
    @static_test { resolution="h<int8>()"                             } function h<T:int8>() : void {}
    @static_test { resolution="h<Foo>()"                              } function h<T:Foo>() : void {}


//////////////////////////////////////////////////////////////////////////////
// main
//////////////////////////////////////////////////////////////////////////////

    function main() : void
    {
        var vint8      : int8              ;
        var vint16     : int16             ;
        var vint32     : int32             ;
        var vint64     : int64             ;
        var vfloat32   : float32           ;
        var vfloat64   : float64           ;
        var vFoo       : Foo               ;
        var vBar       : Bar               ;
        var vBase      : Base              ;
        var vDerived   : Derived           ;
        var vExtended  : Extended          ;
        var vComplex   : Complex<int32>    ;

        // full match after deduction
        @static_test { expect_resolution="f<T>(T)" } f(vint16);
        @static_test { expect_resolution="f<T>(T)" } f(vfloat64);
        @static_test { expect_resolution="f<T>(T)" } f(vBar);

        // specialization is better than generalization
        @static_test { expect_resolution="f<int8>(T)" } f(vint8);
        @static_test { expect_resolution="f<Foo>(T)" } f(vFoo);
        @static_test { expect_resolution="f<Complex<int32>>(T)" } f(vComplex);

        // non-template is better than template
        @static_test { expect_resolution="f(int32)" } f(vint32);

        // can not deduced
        // TODO keep going with unresolved while static test
        @static_test { expect_resolution="" } g(vint32, vint64);

        // fully qualified template instantiation
        @static_test { expect_resolution="g<T>(T,T)" } g<float64>(vfloat64, vfloat64);
        @static_test { expect_resolution="g<T>(T,T)" } g<Bar>(vBar, vBar);
        @static_test { expect_resolution="g<int32>(int32,int32)" } g<int32>(vint32, vint64);
        @static_test { expect_resolution="g<Foo>(Foo,Foo)" } g<Foo>(vFoo, vFoo);

        // no-parameter template function
        @static_test { expect_resolution="h<T>()" } h(vfloat64);
        @static_test { expect_resolution="h<T>()" } h(vint16);
        @static_test { expect_resolution="h<int8>()" } h(vint8);
        @static_test { expect_resolution="h<Foo>()" } h(vFoo);


    }
