class HelloWorld {
	@server // this is executed on server domain
	@export // export keywords allows other domain (eg. client) to call this function
	public static function hello(x:float, y:float, z:float) {
		system.print("receive from client -> (", x, ", ", y, ", ", z, ")\n");
		// by default this send back to the sender
		ack(x, y, z);
	}
	
	@client // this is executed on client domain
	public static function ack(x:float, y:float, z:float) {
		system.print("ack from server -> (", x, ", ", y, ", ", z, ")\n");
		exit(0);
	}
	
	@client
	public static function main(args:Array<String>) {
		if(args != 2) {
			system.print("usage: helloworld <username> <password>\n");
			return;
		}
		
		if(network.join(/*username*/ args[0], /*password*/ args[1])) {
			// calling the "hello" function on server-side
			hello(1.0f, 2.0f, 3.0f);
		}
	}
}