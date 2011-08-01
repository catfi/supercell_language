package com.zillians.tutorials.tutorial2;

class HelloWorldClient {
	@client
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
			HelloWorldServer.hello(1.0f, 2.0f, 3.0f);
		}
	}
}