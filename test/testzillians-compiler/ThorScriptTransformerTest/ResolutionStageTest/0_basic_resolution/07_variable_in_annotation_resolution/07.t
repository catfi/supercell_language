@client
function dummy():void;

function test():void {
	var a:int32;
	
	@target { id = a }
	dummy();
}