@default_domain { domain = "client" }
 
class ClientContext {
	// as we have default_domain specified above, there's no need to write @client here
	public static function main(args:Array<String>) {
		if(network.join(/*username*/ args[0], /*password*/ args[1])) {
			system.sleep(1);
			network.leave();
		}
	}
}
