//////////////////////////////////////////////////////////////////////////////
//
// non-template class
//
//////////////////////////////////////////////////////////////////////////////
@static_test { resolution="Foo"             } class Foo {}
@static_test { resolution="Bar"             } class Bar {}

//////////////////////////////////////////////////////////////////////////////
//
// one type parameter class template
//
//////////////////////////////////////////////////////////////////////////////
@static_test { resolution="Complex<X>"              } class Complex<X> {}
@static_test { resolution="Complex<float64>"        } class Complex<X:float64> {}
@static_test { resolution="Complex<Foo>"            } class Complex<X:Foo> {}
@static_test { resolution="Complex<Complex<int32>>" } class Complex<X:Complex<X:int32> > {}

//////////////////////////////////////////////////////////////////////////////
//
// two type parameters class template
//
//////////////////////////////////////////////////////////////////////////////
// full template
@static_test { resolution="Pair<X,Y>" }       class Pair<X, Y> {}
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
@static_test { resolution="Pair<X:float32,Complex<X:X> >" } class Pair<X:float32, Y:Complex<X:X> > {}

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
@static_test { resolution="Tuple<int32,int32,int32>" } class Tuple<X:int32>, Y:int32, Z:int32> {}
// one specialization with class template
@static_test { resolution="Tuple<X:Complex<X:int32>,Y,Z>" } class Tuple<X:Complex<X:int32>, Y, Z> {}
@static_test { resolution="Tuple<X:Complex<X:int32>,Y:X,Z>" } class Tuple<X:Complex<X:int32>, Y:X, Z> {}
@static_test { resolution="Tuple<X:Complex<X:int32>,Y:X,Z:Y>" } class Tuple<X:Complex<X:int32>, Y:X, Z:Y> {}

//////////////////////////////////////////////////////////////////////////////
//
// funtion to use
//
//////////////////////////////////////////////////////////////////////////////
function f() : void
{
    //// no match
    //@static_test { expect_resolution=""                } var obj : Fooo;

    //////////////////////////////////////////////////////////////////////////////
    //
    // non-template class
    //
    //////////////////////////////////////////////////////////////////////////////
    { @static_test { expect_resolution="Foo"             } var obj : Foo; }

    //////////////////////////////////////////////////////////////////////////////
    //
    // one type parameter class template
    //
    //////////////////////////////////////////////////////////////////////////////
    
    // TODO no match
    
    // Complex<X>
    { @static_test { expect_resolution="Complex<X>"          } var obj : Complex<int8>; }
    { @static_test { expect_resolution="Complex<X>"          } var obj : Complex<int32>; }
    { @static_test { expect_resolution="Complex<X>"          } var obj : Complex<float32>; }
    // Complex<float64>
    { @static_test { expect_resolution="Complex<float64>"    } var obj : Complex<float64>; }
    // Complex<Foo>
    { @static_test { expect_resolution="Complex<Foo>"        } var obj : Complex<Foo>; }
    // Complex<Complex<int32>>
    { @static_test { expect_resolution="Complex<Complex<int32>>" } var obj : Complex<Foo>; }

    // Complex<float64> specialization
    { @static_test { expect_resolution="Complex<float64>"    } var obj6 : Complex<float64>; }

    // Complex<Complex<int32>> specialization
    { @static_test { expect_resolution="Complex<Complex<int32>>" } var obj : Complex<Complex<int32> >; }

    //////////////////////////////////////////////////////////////////////////////
    //
    // two type parameters class template
    //
    //////////////////////////////////////////////////////////////////////////////


    // full template
    { @static_test { expect_resolution="Pair<X,Y>" }       var obj : Pair<float64, float64>; }
    { @static_test { expect_resolution="Pair<X,Y>" }       var obj : Pair<float64, Bar>; }
    // one type partial specialization
    { @static_test { expect_resolution="Pair<int32,Y>" }     var obj : Pair<int32  , float64        > ; }
    { @static_test { expect_resolution="Pair<int32,Y>" }     var obj : Pair<int32  , Bar            > ; }
    { @static_test { expect_resolution="Pair<int32,Y>" }     var obj : Pair<int32  , Complex<int32> > ; }
    { @static_test { expect_resolution="Pair<X,int32>" }     var obj : Pair<float64, int32          > ; }
    { @static_test { expect_resolution=""              }     var obj : Pair<int32  , int32          > ; }
    { @static_test { expect_resolution="Pair<Foo,Y>" }       var obj : Pair<Foo    , float64        > ; }
    { @static_test { expect_resolution="Pair<Foo,Y>" }       var obj : Pair<Foo    , Bar            > ; }
    { @static_test { expect_resolution="Pair<Foo,Y>" }       var obj : Pair<Foo    , Complex<int32> > ; }
    { @static_test { expect_resolution="Pair<X,Foo>" }       var obj : Pair<float64, Foo            > ; }
    { @static_test { expect_resolution="Pair<Foo, Foo>" }    var obj : Pair<Foo    , Foo            > ; }
    // full specialization
    { @static_test { expect_resolution="Pair<int8,int8>" }   var obj : Pair<int8   , int8           > ; }
    { @static_test { expect_resolution="Pair<Foo,Foo>" }     var obj : Pair<Foo    , Foo            > ; }
    // full specilization with type parameter declaration
    { @static_test { expect_resolution="Pair<X:float32,Y:X>" } var obj : Pair<float32, float32> ; }
    // full specilization with type parameter declaration in class template
    { @static_test { expect_resolution="Pair<X:float32,Complex<X:X> >" } var obj : Pair<float32, Complex<float32> > ; }


    //////////////////////////////////////////////////////////////////////////////
    //
    // three type parameters class template
    //
    //////////////////////////////////////////////////////////////////////////////
    // ambi
    { @static_test { resolution=""                     } var obj : Tuple<int8 , int8 , int8 > ; }
    { @static_test { resolution=""                     } var obj : Tuple<int16, int16, int16> ; }
    { @static_test { resolution=""                     } var obj : Tuple<int16, int16, int8 > ; }
    { @static_test { resolution=""                     } var obj : Tuple<int16, int8 , int16> ; }
    { @static_test { resolution=""                     } var obj : Tuple<int8 , int16, int16> ; }
    // full template
    { @static_test { resolution="Tuple<X,Y,Z>" } var obj : Tuple<float64, float64, float64> ; }
    { @static_test { resolution="Tuple<X,Y,Z>" } var obj : Tuple<float64, float32, true> ; }
    { @static_test { resolution="Tuple<X,Y,Z>" } var obj : Tuple<float64, float32, Foo> ; }
    { @static_test { resolution="Tuple<X,Y,Z>" } var obj : Tuple<Bar, Bar, Bar> ; }
    { @static_test { resolution="Tuple<X,Y,Z>" } var obj : Tuple<Bar, Bar, Complex<float64> > ; }
    { @static_test { resolution="Tuple<X,Y,Z>" } var obj : Tuple<Complex<float64>, Bar, Complex<float64> > ; }
    // one type partial specialization
    { @static_test { resolution="Tuple<int8,Y,Z>" } var obj : Tuple<int8, float64, float64> ; }
    { @static_test { resolution="Tuple<X,int8,Z>" } var obj : Tuple<float64, int8, float64> ; }
    { @static_test { resolution="Tuple<X,Y,int8>" } var obj : Tuple<float64, float64, int8> ; }
    { @static_test { resolution="Tuple<X,Y,int8>" } var obj : Tuple<Foo, float64, int8> ; }
    { @static_test { resolution="Tuple<X,Y,int8>" } var obj : Tuple<Foo, Bar, int8> ; }
    { @static_test { resolution="Tuple<X,Y,int8>" } var obj : Tuple<Complex<int64>, Bar, int8> ; }
    // two types partial specialization
    { @static_test { resolution="Tuple<int16,int16,Z>" } var obj : Tuple<int16, int16, float64> ; }
    { @static_test { resolution="Tuple<int16,Y,int16>" } var obj : Tuple<int16, float64, int16> ; }
    { @static_test { resolution="Tuple<Z,int16,int16>" } var obj : Tuple<float64, int16, int16> ; }
    { @static_test { resolution="Tuple<Z,int16,int16>" } var obj : Tuple<Foo, int16, int16> ; }
    { @static_test { resolution="Tuple<Z,int16,int16>" } var obj : Tuple<Complex<int64>, int16, int16> ; }
    // ambugious
    // full specialization
    { @static_test { resolution="Tuple<int32,int32,int32>" } var obj : Tuple<int32, int32, int32> ; }
    // one specialization with class template
    { @static_test { resolution="Tuple<X:Complex<X:int32>,Y,Z>"     } var obj : Tuple<Complex<int32>, float64, float64> ; }
    { @static_test { resolution="Tuple<X:Complex<X:int32>,Y:X,Z>"   } var obj : Tuple<Complex<int32>, Complex<int32>, float64> ; }
    { @static_test { resolution="Tuple<X:Complex<X:int32>,Y:X,Z:Y>" } var obj : Tuple<Complex<int32>, Complex<int32>, Complex<int32> > ; }
}
