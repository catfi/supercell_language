class my_class
{
    var m_data:uint32;
}

function f(y:uint32):void
{
    var x:my_class;
    x.m_data = 13;
    f(x.m_data);
}
