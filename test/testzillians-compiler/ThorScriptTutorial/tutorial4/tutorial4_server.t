@default_domain { domain = "server" }

class PlayerContext {
	var id;
	var x,y:float;
	
	@server
	public function new() {
		// this atomic block guarantee the atomicity of all statement enclosed
		// (note that is is dead-lock-free implementation, we don't want our developers write dead-lock code) 
		atomic {
			id = (global as GlobalContext)++;
		}
		system.print("client ", id, " goes live\n");
	}
	
	@export
	public static function setPosition(x:float, y:float) {
		// self keyword can be used anywhere in any domain
		var p = self as PlayerContext;
		p.x = x;
		p.y = y;
		p.reportLocation();
	}
	
	@export
	public static function moveUp() {
		var p = self as PlayerContext;
		p.y -= 1.0f;
		p.reportLocation();
	}
	
	@export
	public static function moveDown() {
		var p = self as PlayerContext;
		p.y += 1.0f;
		p.reportLocation();
	}

	@export
	public static function moveLeft() {
		var p = self as PlayerContext;
		p.x -= 1.0f;
		p.reportLocation();
	}
	
	@export
	public static function moveRight() {
		var p = self as PlayerContext;
		p.x += 1.0f;
		p.reportLocation();
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
		// as in previous zscript, we have "global" to store any game-related information on server domain
		// and the global variable can be accessed anywhere in server domain
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