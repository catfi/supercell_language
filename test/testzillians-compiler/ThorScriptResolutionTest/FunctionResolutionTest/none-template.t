//////////////////////////////////////////////////////////////////////////////
// some classes
//////////////////////////////////////////////////////////////////////////////

    class Foo {}
    class Bar {}
    class Base {}
    class Derived extends Base {}
    class Extended extends Derived {}

//////////////////////////////////////////////////////////////////////////////
// Function Declarations
//////////////////////////////////////////////////////////////////////////////

    // f()
    @static_test { resolution="f()"                               } function f() : void {}
    @static_test { resolution="f(int32)"                          } function f(a:int32) : void {}
    @static_test { resolution="f(int32,int32)"                    } function f(a:int32, b:int32) : void {}
    @static_test { resolution="f(int32,int32,int32)"              } function f(a:int32, b:int32, c:int32) : void {}

    // g()
    @static_test { resolution="g(int32)"                          } function g(a:int32) : void {}
    @static_test { resolution="g(float32)"                        } function g(a:float32) : void {}

    // h()
    @static_test { resolution="h(int32)"                          } function h(a:int32) : void {}
    @static_test { resolution="h(Foo)"                            } function h(a:Foo) : void {}
    @static_test { resolution="h(int32, int32)"                   } function h(a:int32, b:int32) : void {}

    // i()
    @static_test { resolution="i(float32)"                        } function i(a:float32) : void {}
    @static_test { resolution="i(Foo)"                            } function i(a:Foo) : void {}
    @static_test { resolution="i(int32, int32)"                   } function i(a:int32, b:int32) : void {}

    // j()
    @static_test { resolution="j(int32, int64)"                   } function j(a:int32, b:int64) : void {}
    @static_test { resolution="j(int64, int32)"                   } function j(a:int64, b:int32) : void {}

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

        //// no match
        // TODO keep going with unresolved while static test
        //@static_test { expect_resolution="" } no_such_function();

        // exactly match
        @static_test { expect_resolution="f()"                  } f();
        @static_test { expect_resolution="f(int32)"             } f(vint32);
        @static_test { expect_resolution="f(int32,int32)"       } f(vint32, vint32);
        @static_test { expect_resolution="f(int32,int32,int32)" } f(vint32, vint32, vint32);

        // promotion
        @static_test { expect_resolution="i(float32)" } i(vint16);
        @static_test { expect_resolution="i(float32)" } i(vint8);
        @static_test { expect_resolution="i(float32)" } i(vint64);

        // standard conversion
        @static_test { expect_resolution="h(int32)" } h(vint64);
        @static_test { expect_resolution="h(int32)" } h(vfloat32);
        @static_test { expect_resolution="h(int32)" } h(vfloat64);

        // promotion is better than standard conversion
        @static_test { expect_resolution="g(float32)" } g(vint64);

        // ambi
        // TODO keep going with unresolved while static test
        @static_test { expect_resolution="" } j(vint64, vint64);
        @static_test { expect_resolution="" } j(vint32, vint32);
    }
