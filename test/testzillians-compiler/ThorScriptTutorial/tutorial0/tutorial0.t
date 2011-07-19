package com.zillians.tutorials.tutorial0;

class HelloWorld {
	// note that we can have main function inside client domain
	@client
	public static function main(args:Array<String>) {
		system.print("Hello ThorScript!", " ", "Here comes the land of Asgard!");
	}
}