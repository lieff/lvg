// makeswf -v 7 -s 200x150 -r 1 -o createTextField-returnvalue.swf createTextField-returnvalue.as

myCreateTextField = ASnative (104, 200);
trace (createTextField);
trace (createTextField ("t", 0, 0, 0, 50, 25));
trace (t);
trace (myCreateTextField ("m", 1, 100, 0, 50, 25));
trace (m);

getURL ("fscommand:quit", "");

