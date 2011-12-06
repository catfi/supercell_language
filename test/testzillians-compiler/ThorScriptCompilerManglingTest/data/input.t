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

// primitive-type
function _func_10_(x:int8):void { }
function _func_11_(x:int16):void { }
function _func_12_(x:int32):void { }
function _func_13_(x:int32):void { }
function _func_14_(x:int64):void { }

// non-primitive-type
class _QWE_ { }
function _func_20_(x:_QWE_):void { }

// pointer vs reference
function _func_30_(x:ptr_<void_>):void { }
function _func_31_(x:ref_<int8>):void { }
function _func_32_(x:ptr_<ptr_<void_> >):void { }
function _func_33_(x:ref_<ptr_<void_> >):void { }
function _func_34_(x:ref_<ptr_<ptr_<void_> > >):void { }
//function _func_35_(x:function(int8):void):void { }
//function _func_36_(x:function(int8):void, y:function(int8):void, z:function(int8):void):void { }
function _func_37_(x:_QWE_):void { }
function _func_38_(x:_QWE_, y:_QWE_, z:_QWE_):void { }
//function _func_39_(x:function(_QWE_, _QWE_):void, y:function(_QWE_, _QWE_):void):void { }

// const vs non-const
function _func_40_(x:ptr_<const_<void_> >):void { }
function _func_41_(x:ptr_<void_>):void { }
function _func_42_(x:ptr_<ptr_<const_<void_> > >):void { }
function _func_43_(x:ptr_<const_<ptr_<void_> > >):void { }
function _func_44_(x:ptr_<ptr_<void_> >):void { }

// simple class member
class _BBB_
{
    function _func_51_():void { }
    function _func_52_(x:_BBB_):void { }
    function _func_53_(x:_BBB_, y:_BBB_, z:_BBB_):void { }
}

/*
// templated class member
class _CCC_<T, T2>
{
    function _func_54_(x:_QWE_):void { }
}

// class templated member
class _DDD_
{
    function _func_55_<T, T2>(x:_QWE_):int64 { }
}

// templated function
function _func_56_<T, T2>(x:_QWE_):int64 { }

function main():void
{
    // simple class member
    var bbb:_BBB_;
    bbb._func_51_();
    bbb._func_52_(0);
    bbb._func_53_(0, 0, 0);

    // templated class member
    var ccc:_CCC_<int8, int8>;
    ccc._func_54_(0);

    // templated class member (specialized class)
    var ccc2:_CCC_<_QWE_, _QWE_>;
    ccc2._func_54_(0);

    // class templated member
    var ddd:_DDD_;
    ddd._func_55_<int8, int8>(0);

    // class templated member (specialized member)
    ddd._func_55_<_QWE_, _QWE_>(0);

    // templated function
    _func_56_<int8, int8>(0);

    // templated function (specialized)
    _func_56_<_QWE_, _QWE_>(0);
}
*/

