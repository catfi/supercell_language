package com.zillians.tutorials.tutorial4;

@default_domain { domain = "client" }

// we can have C-style external linkage as native function
@native
function wait_for_input():int; 

class PlayerContext {
	@client
	public function new() {
		
	}
}

class ClientContext {
	@client
	public static function main(args:Array<String>) {
		if(network.join(/*username*/ args[0], /*password*/ args[1])) {
			// the client can call static function in server domain without having object reference on the PlayerContext
			// (as it's static function)
			PlayerContext.setPosition(0.0f, 0.0f);
		}
		
		var input:int;
		while(true) {
			// retrieve user input from external function
			input = wait_for_input();
			if(input == -1) break;
			else
			{
				switch(input) {
				case 8: PlayerContext.moveUp();
				case 4: PlayerContext.moveLeft();
				case 2: PlayerContext.moveDown();
				case 6: PlayerContext.moveRight();
				default: system.print("unknown moving direction\n");
				}
			}
		}
	}
}
