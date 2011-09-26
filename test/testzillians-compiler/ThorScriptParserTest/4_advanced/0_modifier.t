class my_class
{
    // specify visibility only
    public var a:int32;
    private var b:int32;
    protected var c:int32;
    public function f1():void {}
    private function f2():void {}
    protected function f3():void {}

    // specify static-ness only
    static var d:int32;
    static var e:int32;
    static var f:int32;
    static function f4():void {}

    // specify both visibility and static-ness
    public static var g:int32;
    private static var h:int32;
    protected static var i:int32;
    public static function f5():void {}
    private static function f6():void {}
    protected static function f7():void {}
}
