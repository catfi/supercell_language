class my_class
{
    // specify visibility only
    public var a:uint32;
    private var b:uint32;
    protected var c:uint32;
    public function f1():void {}
    private function f2():void {}
    protected function f3():void {}

    // specify static-ness only
    static var d:uint32;
    static var e:uint32;
    static var f:uint32;
    static function f4():void {}

    // specify both visibility and static-ness
    public static var g:uint32;
    private static var h:uint32;
    protected static var i:uint32;
    public static function f5():void {}
    private static function f6():void {}
    protected static function f7():void {}
}
