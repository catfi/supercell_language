//var a:uint32;
//var b:uint32 = -13;

class C
{
 function f0(x:int32, y:int32):int32 {
 print("hello", 
 x, 
   y);
  }
}

typedef C CX;

function f0(x
:int32, y:int32):void
{
  var _x:int32 = x;
  var _y:int32 = y;
  return x + y;
}
