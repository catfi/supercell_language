class QWE {}
@static_test { expect_message={ level="LEVEL_ERROR", id="DUPE_NAME", parameters={ id="QWE" } } }
class QWE {}

var x:uint32;
@static_test { expect_message={ level="LEVEL_ERROR", id="DUPE_NAME", parameters={ id="x" } } }
var x:uint32;

function f():void {}
@static_test { expect_message={ level="LEVEL_ERROR", id="DUPE_NAME", parameters={ id="f" } } }
function f():void {}

/*
@static_test { expect_message={ level="LEVEL_ERROR", id="DUPE_NAME", parameters={ id="a" } } }
function g(a:uint32, a:uint32):void {}
*/

@static_test { expect_message={ level="LEVEL_ERROR", id="DUPE_NAME", parameters={ id="T" } } }
function g2<T, T>():void {}
