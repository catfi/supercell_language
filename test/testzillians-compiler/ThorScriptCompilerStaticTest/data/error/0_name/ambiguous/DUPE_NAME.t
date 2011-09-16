var x:uint32;

@static_test { expect_message={ level="LEVEL_ERROR", id="DUPE_NAME", parameter={ ID="x" } } }
var x:uint32;
