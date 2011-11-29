class ptr_<T>
{
}

class ref_<T>
{
}

class const_<T>
{
}

class void_
{
}

// no-param vs params vs return-value
function _func_00_():void { }
function _func_01_(x:int8, y:int8):void { }
/*  NOTE: return type makes no difference in signature
    function _func_02_():int8 { }
*/

// primitive-type
function _func_10_(x:int8):void { }
function _func_11_(x:int16):void { }
function _func_12_(x:int32):void { }
function _func_13_(x:int32):void { }
function _func_14_(x:int64):void { }

// non-primitive-type
class QWE { }
function _func_20_(x:QWE):void { }
/*  NOTE: return type makes no difference in signature
    function _func_21_(x:QWE):QWE { }
*/

// pointer vs reference
function _func_30_(x:ptr_<void_>):void { }
function _func_31_(x:ref_<int8>):void { }
function _func_32_(x:ptr_<ptr_<void_> >):void { }
function _func_33_(x:ref_<ptr_<void_> >):void { }
function _func_34_(x:ref_<ptr_<ptr_<void_> > >):void { }
function _func_35_(x:function(int8):void):void { }
function _func_36_(x:function(int8):void, x2:function(int8):void):void { }

// const vs non-const
function _func_40_(x:ptr_<const_<void_> >):void { }
function _func_41_(x:ptr_<void_>):void { }
function _func_42_(x:ptr_<ptr_<const_<void_> > >):void { }
function _func_43_(x:ptr_<const_<ptr_<void_> > >):void { }
function _func_44_(x:ptr_<ptr_<void_> >):void { }

class ASD
{
    function _func_45_():void { }
/*  NOTE: same signature as _func_45_
    static function _func_46_():void { }
*/
}

