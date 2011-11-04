// as we have default_domain specified above, there's no need to write @server for every function below
@default_domain { domain = "server" }

class GlobalContext {
	var message;
	
	// "game_init" handler is invoked when the game is deployed and started/loaded
	@handler { name = "game_init" }
	public static function initialize() {
		var ctx = new GlobalContext;
		ctx.message = "game initialized";
		ctx.print_message();
		global = ctx;
	}
	
	// "game_kill" handler is invoked when the game is stopped/unloaded
	@handler { name = "game_kill" }
	public static function kill() {
		var ctx = global as GlobalContext;
		ctx.message = "game killed";
		ctx.print_message();
	}
	
	// "session_open" handler will be invoked when client connected
	@handler { name = "session_open" }
	public static function client_connected() {
		system.print("client connected\n");
	}
	
	// "session_close" handler will be invoked when client disconnected
	@handler { name = "session_close" }
	public static function client_disconnected() {
		system.print("client disconnected\n");
	}
	
	// note that the function is not "static" function, we we have this pointer in this case (just like traditional class/object)
	public function print_message() {
		system.print(message, "\n");
	}
}