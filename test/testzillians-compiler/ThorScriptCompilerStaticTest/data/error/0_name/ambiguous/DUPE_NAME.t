// type dupe
class QWE {}
@static_test { expect_message={ level="LEVEL_ERROR", id="DUPE_NAME", parameters={ id="QWE" } } }
class QWE {}

// global var dupe
var x:uint32;
@static_test { expect_message={ level="LEVEL_ERROR", id="DUPE_NAME", parameters={ id="x" } } }
var x:uint32;

// function dupe
function f():void {}
@static_test { expect_message={ level="LEVEL_ERROR", id="DUPE_NAME", parameters={ id="f" } } }
function f():void {}

// arg dupe
@static_test { expect_message={ level="LEVEL_ERROR", id="DUPE_NAME", parameters={ id="a" } } }
function g(a:uint32, a:uint32):void {}

// template arg dupe
@static_test { expect_message={ level="LEVEL_ERROR", id="DUPE_NAME", parameters={ id="T" } } }
function g2<T, T>():void {}

// local var dupe
function h(a:uint32):void
{
    @static_test { expect_message={ level="LEVEL_ERROR", id="DUPE_NAME", parameters={ id="a" } } }
    var a:uint32;

    var b:uint32;
    @static_test { expect_message={ level="LEVEL_ERROR", id="DUPE_NAME", parameters={ id="b" } } }
    var b:uint32;
}
