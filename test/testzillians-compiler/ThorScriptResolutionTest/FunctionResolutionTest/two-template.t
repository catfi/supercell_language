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
// 2 template functions
//////////////////////////////////////////////////////////////////////////////

    // general, general
    @static_test { resolution="f<T,U>(T,U)"                           } function f<T, U>(a:T, b:U) : void {}

    // general, non-template
    @static_test { resolution="f<T>(T,int16)"                         } function f<T>(a:T, b:int16) : void {}

    // special, special
    @static_test { resolution="f<int16,int16>(int16,int16)"           } function f<T:int16, U:int16>(a:T, b:U) : void {}

    // non-template, non-template
    @static_test { resolution="f(int16,int16)"                        } function f(a:int16, b:int16) : void {}

    // g
    @static_test { resolution="g<T>(T,T)"                             } function g<T>(a:T, b:T) : void {}
    @static_test { resolution="g<T,U>(T,U)"                           } function g<T,U>(a:T, b:U) : void {}

    @static_test { resolution="g<int16>(T,T)"                         } function g<T:int16>(a:T, b:T) : void {}
    @static_test { resolution="g<int16,int16>(T,U)"                   } function g<T:int16, U:int16>(a:T, b:U) : void {}

    // h
    @static_test { resolution="h<T>(T,T)"                             } function h<T>(a:T, b:T) : void {}

    // r
    @static_test { resolution="r(int32,int8)"                         } function r<T>(a:int32, b:int8) : void {}
    @static_test { resolution="r<T>(T,int32)"                         } function r<T>(a:T, b:int32) : void {}
    @static_test { resolution="r<T:int32>(T,int32)"                   } function r<T:int32>(a:T, b:int32) : void {}

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
        @static_test { expect_resolution="f<T,U>(T,U)" } f(vint16, vint64);
        @static_test { expect_resolution="f<T,U>(T,U)" } f(vfloat64, vFoo);
        @static_test { expect_resolution="f<T,U>(T,U)" } f(vBar, vBase);

        // special is better than general
        @static_test { expect_resolution="f<T>(T,int16)" } f(vBar, vint16);

        // lower degree of freedom is better than higher degree of freedom while full match specialization
        @static_test { expect_resolution="g<int16>(T,T)" } g(vint16, vint16);

        // non-template is better then special
        @static_test { expect_resolution="f(int16,int16)" } f(vint16, vint16);

        // can not deduced
        @static_test { expect_resolution="",
                       expect_message={level="LEVEL_ERROR", id="UNDEFINED_SYMBOL_INFO", parameters={id="h"}}
        } h(vint32, vint64);

        // fully qualified template instantiation
        @static_test { expect_resolution="f<T,U>(T,U)" } f<int64, int8>(vint32, vint64);
        @static_test { expect_resolution="f<int16,int16>(int16,int16)" } f<int16, int16>(vint32, vint64);

        // conversion rank is most important
        @static_test { expect_resolution="r<T:int32>(T,int32)" } r(vint32, vint16);


    }
