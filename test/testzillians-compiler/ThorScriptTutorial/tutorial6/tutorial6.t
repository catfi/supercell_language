package com.zillians.tutorials.tutorial6;

@default_domain { domain = "generic" }

/**
 * This tutorial shows how to write a simple chat room 
 */
 
class SimpleRoom<T> {
	var clients:Set<T>;
	
	public function new() {
		clients = new Set<T>();
	}
	
	public function join(client:Object) {
		if(client instanceof T)
			clients.add(client);
		else
			system.print("client object type mismatched\n");
	}
	
	public function leave(client:Object) {
		if(client instanceof T)
			clients.remove(client);
		else
			system.print("client object type mismatched\n");
	}
	
	public function broadcast(message:String) {
		foreach(var i in clients) {
			i.onMessage(message);
		}
	}
}

interface SimpleClient {
	@client
	public function onMessage(message:String);
}

class Player implements SimpleClient {
	@server
	@client
	var room:Room;
	
	@client
	public function new() {
	}
	
	@client
	public function onMessage(message:String) {
		system.print("message received: ", message, "\n");
		if(message.compare("start") == 0) {
			// game logic starts here
		}
		else if(message.compare("stop") == 0) {
			exit(0);
		}
	}
	
	@client
	public static function main(args:Array<String>) {
		var p;
		if(network.join(/*username*/ args[0], /*password*/ args[1])) {
			p = self as Player;
		}
		
		while(true) {
			var input:String = wait_for_input();
			room.broadcast(input);
		}
	}
} 

class Room extends SimpleRoom<Player> {
	public function new() {
	}
}

class GlobalContext {
	var next_client_id:int;
	var rooms:Array<Room>;

	public function new(var total_number_of_rooms:int) {
		next_client_id = 0;
		rooms = new Array<Room>(total_number_of_rooms);
		for(var i in 0...total_number_of_rooms) {
			rooms[i] = new Room();
		}
	}

	@handler { name = "game_init" }
	public static function initialize() {
		var ctx = new GlobalContext(10);
		global = ctx;
	}
	
	@handler { name = "game_kill" }
	public static function kill() {
	}
	
	@handler { name = "session_open" }
	public static function client_connected() {
		var ctx = global as GlobalContext;
		
		var player = new Player();
		var room = ctx.rooms[math.rand() % 10]; // randomly choose a room to join
		room.join(player);
		player.room = room; 
		self = player;
	}
	
	@handler { name = "session_close" }
	public static function client_disconnected() {
		system.print("client disconnected\n");
		var player = self as Player;
		player.room.leave(player);
	}
}