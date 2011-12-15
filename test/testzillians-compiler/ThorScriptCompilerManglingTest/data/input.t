class ptr_<T>   { }
class ref_<T>   { }
class const_<T> { }
class void_     { }

class _QWE_ { }

// no-param vs params vs return-value
function _func_00_():void               { }
function _func_01_(x:int8, y:int8):void { }

// primitive-type
function _func_10_(x:int8):void  { }
function _func_11_(x:int16):void { }
function _func_12_(x:int32):void { }
function _func_13_(x:int32):void { }
function _func_14_(x:int64):void { }

// non-primitive-type
function _func_20_(x:_QWE_):void { }

// pointer vs reference
function _func_30_(x:ptr_<void_>):void                                                       { }
function _func_31_(x:ref_<int8>):void                                                        { }
function _func_32_(x:ptr_<ptr_<void_> >):void                                                { }
function _func_33_(x:ref_<ptr_<void_> >):void                                                { }
function _func_34_(x:ref_<ptr_<ptr_<void_> > >):void                                         { }
function _func_35_(x:function(int8):void):void                                               { }
function _func_36_(x:function(int8):void, y:function(int8):void, z:function(int8):void):void { }
function _func_37_(x:_QWE_):void                                                             { }
function _func_38_(x:_QWE_, y:_QWE_, z:_QWE_):void                                           { }
function _func_39_(x:function(_QWE_, _QWE_):void, y:function(_QWE_, _QWE_):void):void        { }

// const vs non-const
function _func_40_(x:ptr_<const_<void_> >):void        { }
function _func_41_(x:ptr_<void_>):void                 { }
function _func_42_(x:ptr_<ptr_<const_<void_> > >):void { }
function _func_43_(x:ptr_<const_<ptr_<void_> > >):void { }
function _func_44_(x:ptr_<ptr_<void_> >):void          { }

/*
// namespace member
namespace _AAA_
{
    namespace _XXX_
    {
        function _func_50_():void { }
    }

    class _ZXC_<T>
    {
        void _func_51_() { }
    };
}
*/

// class member
class _BBB_
{
    public function _func_60_():void                          { }
    public function _func_61_(x:_QWE_):void                   { }
    public function _func_62_(x:_QWE_, y:_QWE_, z:_QWE_):void { }
    public function _func_63_(x:_BBB_, y:_BBB_, z:_BBB_):void { }
}

// templated class member
/*
class _CCC_<T, T2>
{
    public function _func_70_():void                          { }
    public function _func_71_(x:_QWE_):void                   { }
    public function _func_72_(x:_QWE_, y:_QWE_, z:_QWE_):void { }
    public function _func_73_(x:_CCC_<T, T2>, y:_CCC_<T, T2>, z:_CCC_<T, T2>):void { }
}
*/
class _CCC_<T:int8, T2:int8>
{
    public function _func_70_():void                          { }
    public function _func_71_(x:_QWE_):void                   { }
    public function _func_72_(x:_QWE_, y:_QWE_, z:_QWE_):void { }
    public function _func_73_(x:_CCC_<T, T2>, y:_CCC_<T, T2>, z:_CCC_<T, T2>):void { }
}
class _CCC_<T:_QWE_, T2:_QWE_>
{
    public function _func_70_():void                          { }
    public function _func_71_(x:_QWE_):void                   { }
    public function _func_72_(x:_QWE_, y:_QWE_, z:_QWE_):void { }
    public function _func_73_(x:_CCC_<T, T2>, y:_CCC_<T, T2>, z:_CCC_<T, T2>):void { }
}

// class templated member
class _DDD_
{
    /*
    public function _func_80_<T, T2>():int64                          { }
    public function _func_81_<T, T2>(x:_QWE_):int64                   { }
    public function _func_82_<T, T2>(x:_QWE_, y:_QWE_, z:_QWE_):int64 { }
    public function _func_83_<T, T2>(x:_DDD_, y:_DDD_, z:_DDD_):int64 { }
    */
    public function _func_80_<T:int8, T2:int8>():int64                            { }
    public function _func_81_<T:int8, T2:int8>(x:_QWE_):int64                     { }
    public function _func_82_<T:int8, T2:int8>(x:_QWE_, y:_QWE_, z:_QWE_):int64   { }
    public function _func_83_<T:int8, T2:int8>(x:_DDD_, y:_DDD_, z:_DDD_):int64   { }
    public function _func_80_<T:_QWE_, T2:_QWE_>():int64                          { }
    public function _func_81_<T:_QWE_, T2:_QWE_>(x:_QWE_):int64                   { }
    public function _func_82_<T:_QWE_, T2:_QWE_>(x:_QWE_, y:_QWE_, z:_QWE_):int64 { }
    public function _func_83_<T:_QWE_, T2:_QWE_>(x:_DDD_, y:_DDD_, z:_DDD_):int64 { }
}

// templated function
/*
function _func_90_<T, T2>():int64                          { }
function _func_91_<T, T2>(x:_QWE_):int64                   { }
function _func_92_<T, T2>(x:_QWE_, y:_QWE_, z:_QWE_):int64 { }
*/
function _func_90_<T:int8, T2:int8>():int64                            { }
function _func_91_<T:int8, T2:int8>(x:_QWE_):int64                     { }
function _func_92_<T:int8, T2:int8>(x:_QWE_, y:_QWE_, z:_QWE_):int64   { }
function _func_90_<T:_QWE_, T2:_QWE_>():int64                          { }
function _func_91_<T:_QWE_, T2:_QWE_>(x:_QWE_):int64                   { }
function _func_92_<T:_QWE_, T2:_QWE_>(x:_QWE_, y:_QWE_, z:_QWE_):int64 { }

// mixing pointer non-pointer
@call_by_value
function _func_A0_(x:ptr_<_QWE_>, y:_QWE_):void { }
class _EEE_
{
    @call_by_value
    function _func_A1_(x:ptr_<_QWE_>, y:_QWE_):void { }
}

function main():void
{
    var qwe:_QWE_;

    /*
    // namespace member
    _AAA_._XXX_._func_50_();
    var zxc:_AAA_._ZXC_<_AAA_._XXX_._ASD_>;
    zxc._func_51_();
    */

    /*
    // class member
    var bbb:_BBB_;
    bbb._func_60_();
    bbb._func_61_(qwe);
    bbb._func_62_(qwe, qwe, qwe);
    bbb._func_63_(bbb, bbb, bbb);

    // templated class member
    var ccc:_CCC_<int8, int8>;
    ccc._func_70_();
    ccc._func_71_(qwe);
    ccc._func_72_(qwe, qwe, qwe);
    ccc._func_73_(ccc, ccc, ccc);
    var ccc2:_CCC_<_QWE_, _QWE_>;
    ccc2._func_70_();
    ccc2._func_71_(qwe);
    ccc2._func_72_(qwe, qwe, qwe);
    ccc2._func_73_(ccc2, ccc2, ccc2);

    // class templated member
    var ddd:_DDD_;
    ddd._func_80_<int8, int8>();
    ddd._func_81_<int8, int8>(qwe);
    ddd._func_82_<int8, int8>(qwe, qwe, qwe);
    ddd._func_83_<int8, int8>(ddd, ddd, ddd);
    ddd._func_80_<_QWE_, _QWE_>();
    ddd._func_81_<_QWE_, _QWE_>(qwe);
    ddd._func_82_<_QWE_, _QWE_>(qwe, qwe, qwe);
    ddd._func_83_<_QWE_, _QWE_>(ddd, ddd, ddd);

    // templated function
    _func_90_<int8, int8>();
    _func_91_<int8, int8>(qwe);
    _func_92_<int8, int8>(qwe, qwe, qwe);
    _func_90_<_QWE_, _QWE_>();
    _func_91_<_QWE_, _QWE_>(qwe);
    _func_92_<_QWE_, _QWE_>(qwe, qwe, qwe);
    */

    /*
    // mixing pointer non-pointer
    _func_A0_(qwe, qwe);
    var eee:_EEE_;
    eee._func_A1_(qwe, qwe);
    */
}

