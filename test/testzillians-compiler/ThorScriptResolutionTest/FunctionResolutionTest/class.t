interface I0
{
    @static_test { resolution="I0::x" }
    function x() : void;
}

interface I1
{
    @static_test { resolution="I1::y" }
    function y() : void;
}

interface I2 extends I0, extends I1
{
    @static_test { resolution="I2::y" }
    function y() : void;
}


@static_test { resolution="::a" }
function a() : void {}

class Base implements I2
{
    @static_test { resolution="Base::a" }
    function a() : void {}
    @static_test { resolution="Base::b" }
    function b() : void
    {
        @static_test { expect_resolution="I2::y" }
        y();
        @static_test { expect_resolution="Base::a" }
        this.a();
        @static_test { expect_resolution="", expect_message={level="LEVEL_ERROR", id="UNDEFINED_SYMBOL_INFO", parameters={id="a"}}  }
        a();
    }
}

class Derived extends Base
{
    @static_test { expect="Derived::c" }
    function c() : void { }
}

class Extended extends Derived
{
    @static_test { expect="Derived::c" }
    function c() : void
    {
        @static_test { expect_resolution="Derived::c" }
        super.c();
    }
}

function main() : void
{
    var v : Base;
    v.b();

    var v1 : Derived;
    @static_test { expect_resolution="Base::b" } v1.b();
    @static_test { expect_resolution="I0::x" } v1.x();
    @static_test { expect_resolution="I2::y" } v1.y();

    var v2 : I1;
    //@static_test { expect_resolution="", expect_message={level="LEVEL_ERROR", id="UNDEFINED_SYMBOL_INFO", parameters={id="g"}}  } v2.x();
    @static_test { expect_resolution="I1::y" } v2.y();
}
