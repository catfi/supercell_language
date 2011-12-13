import yoco;
import b;   // file
import c;   // dir has c/c1.t c/c2.t c/c3.t
import d1 = d.d1; // file
import . = e.e1; // dir has e/e11.t e/e12.t e/e13.t

@server
function funABC() : int32
{
    //var xyz : int32 = 0;
    //var abc : int32 = 0;
    //return xyz + abc;
}

@client
function funcClient() : int32
{
    //var xyz : int32 = 0;
    //var abc : int32 = 0;
    //return xyz + abc;
}
