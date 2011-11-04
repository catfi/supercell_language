class HelloWorldServer {
	@server
	@export
	public static function hello(x:float, y:float, z:float) {
		system.print("receive from client -> (", x, ", ", y, ", ", z, ")\n");
		HelloWorldClient.ack(x, y, z);
	}
}