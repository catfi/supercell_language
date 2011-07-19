package com.zillians.tutorials.tutorial5;

@native
function wait_for_input():int; 

class ClientContext {
	@client
	public static function main(args:Array<String>) {
		var p;
		if(network.join(/*username*/ args[0], /*password*/ args[1])) {
		
			// self can be used in client script as well because self object will be automatically replicated (unless specified otherwise)
			p = self as PlayerContext;
			p.setPosition(0.0f, 0.0f);
		}
		
		while(true) {
			var input = wait_for_input();
			if(input == -1) break;
			else
			{
				if(p == null) {
					system.print("failed to get PlayerContext\n");
					exit(-1);
				}
				
				switch(input) {
				case 8: p.moveUp();
				case 4: p.moveLeft();
				case 2: p.moveDown();
				case 6: p.moveRight();
				default: system.print("unknown moving direction\n"); continue;
				}
			}
		}
	}
}
