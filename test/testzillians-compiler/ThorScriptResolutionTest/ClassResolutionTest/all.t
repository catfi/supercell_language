//////////////////////////////////////////////////////////////////////////////
//
// non-template class
//
//////////////////////////////////////////////////////////////////////////////
@static_test { resolution="Foo"             } class Foo {}
@static_test { resolution="Bar"             } class Bar {}
@static_test { resolution="Base"            } class Base {}
@static_test { resolution="Derived"         } class Derived extends Base {}
@static_test { resolution="Extended"        } class Extended extends Derived {}

//////////////////////////////////////////////////////////////////////////////
//
// one type parameter class template
//
//////////////////////////////////////////////////////////////////////////////
@static_test { resolution="Complex<T>"              } class Complex<T> {}
@static_test { resolution="Complex<float64>"        } class Complex<T:float64> {}
@static_test { resolution="Complex<Foo>"            } class Complex<T:Foo> {}
@static_test { resolution="Complex<Complex<int32>>" } class Complex<T:Complex<T:int32> > {}

//////////////////////////////////////////////////////////////////////////////
//
// two type parameters class template
//
//////////////////////////////////////////////////////////////////////////////
// full template
@static_test { resolution="Pair<T,Y>" }       class Pair<X, Y> {}
// one type partial specialization
@static_test { resolution="Pair<int32,Y>" }   class Pair<X:int32, Y      > {}
@static_test { resolution="Pair<X,int32>" }   class Pair<X      , Y:int32> {}
@static_test { resolution="Pair<Foo,Y>" }     class Pair<X:Foo  , Y      > {}
@static_test { resolution="Pair<X,Foo>" }     class Pair<X      , Y:Foo  > {}
// full specialization
@static_test { resolution="Pair<int8,int8>" } class Pair<X:int8, Y:int8> {}
@static_test { resolution="Pair<Foo,Foo>" } class Pair<X:Foo, Y:Foo> {}
// full specilization with type parameter declaration
@static_test { resolution="Pair<X:float32,Y:X>" } class Pair<X:float32, Y:X> {}
// full specilization with type parameter declaration in class template
@static_test { resolution="Pair<X:float32,Complex<T:X> >" } class Pair<X:float32, Y:Complex<T:X> > {}

//////////////////////////////////////////////////////////////////////////////
//
// three type parameters class template
//
//////////////////////////////////////////////////////////////////////////////
// full template
@static_test { resolution="Tuple<X,Y,Z>" } class Tuple<X, Y, Z> {}
// one type partial specialization
@static_test { resolution="Tuple<int8,Y,Z>" } class Tuple<X:int8, Y, Z> {}
@static_test { resolution="Tuple<X,int8,Z>" } class Tuple<X, Y:int8, Z> {}
@static_test { resolution="Tuple<X,Y,int8>" } class Tuple<X, Y, Z:int8> {}
// two types partial specialization
@static_test { resolution="Tuple<int16,int16,Z>" } class Tuple<X:int16, Y:int16, Z> {}
@static_test { resolution="Tuple<int16,Y,int16>" } class Tuple<X:int16, Y, Z:int16> {}
@static_test { resolution="Tuple<Z,int16,int16>" } class Tuple<X, Y:int16, Z:int16> {}
// full specialization
@static_test { resolution="Tuple<int32,int32,int32>" } class Tuple<X:int32, Y:int32, Z:int32> {}
// one specialization with class template
@static_test { resolution="Tuple<X:Complex<T:int32>,Y,Z>" } class Tuple<X:Complex<T:int32>, Y, Z> {}
@static_test { resolution="Tuple<X:Complex<T:int32>,Y:X,Z>" } class Tuple<X:Complex<T:int32>, Y:X, Z> {}
@static_test { resolution="Tuple<X:Complex<T:int32>,Y:X,Z:Y>" } class Tuple<X:Complex<T:int32>, Y:X, Z:Y> {}

//////////////////////////////////////////////////////////////////////////////
//
// funtion to use
//
//////////////////////////////////////////////////////////////////////////////
function f() : void
{
    //// no match
    // TODO keep going with unresolved while static test
    //@static_test { expect_resolution=""                } var obj0 : Fooo;

    //////////////////////////////////////////////////////////////////////////////
    //
    // non-template class
    //
    //////////////////////////////////////////////////////////////////////////////
    { @static_test { expect_resolution="Foo"             } var obj1 : Foo; }

    //////////////////////////////////////////////////////////////////////////////
    //
    // one type parameter class template
    //
    //////////////////////////////////////////////////////////////////////////////
    
    // TODO no match
    
    // Complex<T>
    { @static_test { expect_resolution="Complex<T>"          } var obj2 : Complex<int8>; }
    { @static_test { expect_resolution="Complex<T>"          } var obj3 : Complex<int32>; }
    { @static_test { expect_resolution="Complex<T>"          } var obj4 : Complex<float32>; }
    // Complex<float64>
    { @static_test { expect_resolution="Complex<float64>"    } var obj5 : Complex<float64>; }
    // Complex<Foo>
    { @static_test { expect_resolution="Complex<Foo>"        } var obj6 : Complex<Foo>; }
    // Complex<Complex<int32>>
    { @static_test { expect_resolution="Complex<Complex<int32>>" } var obj7 : Complex<Foo>; }

    // Complex<float64> specialization
    { @static_test { expect_resolution="Complex<float64>"    } var obj8 : Complex<float64>; }

    // Complex<Complex<int32>> specialization
    { @static_test { expect_resolution="Complex<Complex<int32>>" } var obj9 : Complex<Complex<int32> >; }

    //////////////////////////////////////////////////////////////////////////////
    //
    // two type parameters class template
    //
    //////////////////////////////////////////////////////////////////////////////


    // full template
    { @static_test { expect_resolution="Pair<X,Y>" }       var obj10 : Pair<float64, float64>; }
    { @static_test { expect_resolution="Pair<X,Y>" }       var obj11 : Pair<float64, Bar>; }
    // one type partial specialization
    { @static_test { expect_resolution="Pair<int32,Y>" }     var obj12 : Pair<int32  , float64        > ; }
    { @static_test { expect_resolution="Pair<int32,Y>" }     var obj13 : Pair<int32  , Bar            > ; }
    { @static_test { expect_resolution="Pair<int32,Y>" }     var obj14 : Pair<int32  , Complex<int32> > ; }
    { @static_test { expect_resolution="Pair<X,int32>" }     var obj15 : Pair<float64, int32          > ; }
    // TODO keep going with unresolved while static test
    //{ @static_test { expect_resolution=""              }     var obj16 : Pair<int32  , int32          > ; }
    { @static_test { expect_resolution="Pair<Foo,Y>" }       var obj17 : Pair<Foo    , float64        > ; }
    { @static_test { expect_resolution="Pair<Foo,Y>" }       var obj18 : Pair<Foo    , Bar            > ; }
    { @static_test { expect_resolution="Pair<Foo,Y>" }       var obj19 : Pair<Foo    , Complex<int32> > ; }
    { @static_test { expect_resolution="Pair<X,Foo>" }       var obj20 : Pair<float64, Foo            > ; }
    { @static_test { expect_resolution="Pair<Foo, Foo>" }    var obj21 : Pair<Foo    , Foo            > ; }
    // full specialization
    { @static_test { expect_resolution="Pair<int8,int8>" }   var obj22 : Pair<int8   , int8           > ; }
    { @static_test { expect_resolution="Pair<Foo,Foo>" }     var obj23 : Pair<Foo    , Foo            > ; }
    // full specilization with type parameter declaration
    { @static_test { expect_resolution="Pair<X:float32,Y:X>" } var obj24 : Pair<float32, float32> ; }
    // full specilization with type parameter declaration in class template
    { @static_test { expect_resolution="Pair<X:float32,Complex<T:X> >" } var obj25 : Pair<float32, Complex<float32> > ; }


    //////////////////////////////////////////////////////////////////////////////
    //
    // three type parameters class template
    //
    //////////////////////////////////////////////////////////////////////////////
    // TODO keep going with unresolved while static test
    // ambi
    //{ @static_test { resolution=""                     } var obj26 : Tuple<int8 , int8 , int8 > ; }
    //{ @static_test { resolution=""                     } var obj27 : Tuple<int16, int16, int16> ; }
    //{ @static_test { resolution=""                     } var obj28 : Tuple<int16, int16, int8 > ; }
    //{ @static_test { resolution=""                     } var obj29 : Tuple<int16, int8 , int16> ; }
    //{ @static_test { resolution=""                     } var obj30 : Tuple<int8 , int16, int16> ; }
    // full template
    { @static_test { resolution="Tuple<X,Y,Z>" } var obj31 : Tuple<float64, float64, float64> ; }
    { @static_test { resolution="Tuple<X,Y,Z>" } var obj32 : Tuple<float64, float32, Extended> ; }
    { @static_test { resolution="Tuple<X,Y,Z>" } var obj33 : Tuple<float64, float32, Foo> ; }
    { @static_test { resolution="Tuple<X,Y,Z>" } var obj34 : Tuple<Bar, Bar, Bar> ; }
    { @static_test { resolution="Tuple<X,Y,Z>" } var obj35 : Tuple<Bar, Bar, Complex<float64> > ; }
    { @static_test { resolution="Tuple<X,Y,Z>" } var obj36 : Tuple<Complex<float64>, Bar, Complex<float64> > ; }
    // one type partial specialization
    { @static_test { resolution="Tuple<int8,Y,Z>" } var obj37 : Tuple<int8, float64, float64> ; }
    { @static_test { resolution="Tuple<X,int8,Z>" } var obj38 : Tuple<float64, int8, float64> ; }
    { @static_test { resolution="Tuple<X,Y,int8>" } var obj39 : Tuple<float64, float64, int8> ; }
    { @static_test { resolution="Tuple<X,Y,int8>" } var obj40 : Tuple<Foo, float64, int8> ; }
    { @static_test { resolution="Tuple<X,Y,int8>" } var obj41 : Tuple<Foo, Bar, int8> ; }
    { @static_test { resolution="Tuple<X,Y,int8>" } var obj42 : Tuple<Complex<int64>, Bar, int8> ; }
    // two types partial specialization
    { @static_test { resolution="Tuple<int16,int16,Z>" } var obj43 : Tuple<int16, int16, float64> ; }
    { @static_test { resolution="Tuple<int16,Y,int16>" } var obj44 : Tuple<int16, float64, int16> ; }
    { @static_test { resolution="Tuple<Z,int16,int16>" } var obj45 : Tuple<float64, int16, int16> ; }
    { @static_test { resolution="Tuple<Z,int16,int16>" } var obj46 : Tuple<Foo, int16, int16> ; }
    { @static_test { resolution="Tuple<Z,int16,int16>" } var obj47 : Tuple<Complex<int64>, int16, int16> ; }
    // TODO no match
    // ambugious
    // full specialization
    { @static_test { resolution="Tuple<int32,int32,int32>" } var obj48 : Tuple<int32, int32, int32> ; }
    // one specialization with class template
    { @static_test { resolution="Tuple<X:Complex<T:int32>,Y,Z>"     } var obj49 : Tuple<Complex<int32>, float64, float64> ; }
    { @static_test { resolution="Tuple<X:Complex<T:int32>,Y:X,Z>"   } var obj50 : Tuple<Complex<int32>, Complex<int32>, float64> ; }
    { @static_test { resolution="Tuple<X:Complex<T:int32>,Y:X,Z:Y>" } var obj51 : Tuple<Complex<int32>, Complex<int32>, Complex<int32> > ; }
}
