// makeswf -v 7 -s 100x150 -r 15 -o crash-0.6.6-variable-leak-7.swf crash-0.6.6-variable-leak.as

var x = new Array ();
ASSetNativeAccessor (x, 666, "1,2,3,4,5,6", 1);
trace ("OK");

getURL ("fscommand:quit", "");
