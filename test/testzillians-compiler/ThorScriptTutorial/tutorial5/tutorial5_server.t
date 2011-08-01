package com.zillians.tutorials.tutorial5;

@default_domain { domain = "server" }

class PlayerContext {
	@server
	var id;
	@server
	var x,y:float;
	
	public function new() {
		atomic {
			id = (global as GlobalContext)++;
		}
		system.print("client ", id, " goes live\n");
	}
	
	@export
	public function setPosition(x:float, y:float) {
		this.x = x;
		this.y = y;
		reportLocation();
	}
	
	@export
	public function moveUp() {
		y -= 1.0f;
		reportLocation();
	}
	
	@export
	public function moveDown() {
		y += 1.0f;
		reportLocation();
	}

	@export
	public function moveLeft() {
		x -= 1.0f;
		reportLocation();
	}
	
	@export
	public function moveRight() {
		x += 1.0f;
		reportLocation();
	}
	
	private function reportLocation() {
		system.print("client ", id, " is at (", x, ",", y, ")\n");
	}
}

class GlobalContext {
	var next_client_id:int;
	var clients:List<PlayerContext>;

	public function new() {
		next_client_id = 0;
		clients = new List<PlayerContext>();
	}
		
	@handler { name = "game_init" }
	public static function initialize() {
		var ctx = new GlobalContext;
		global = ctx;
	}
	
	@handler { name = "game_kill" }
	public static function kill() {
		var ctx = global as GlobalContext;
	}
	
	@handler { name = "session_open" }
	public static function client_connected() {
		var ctx = global as GlobalContext;
		
		var player = new PlayerContext();
		ctx.clients.append(player);
		self = player;
	}
	
	@handler { name = "session_close" }
	public static function client_disconnected() {
		system.print("client disconnected\n");
		self = null;
	}
}