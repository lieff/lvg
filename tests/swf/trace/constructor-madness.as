// makeswf -v 7 -s 200x150 -r 1 -o constructor-madness.swf constructor-madness.as

trace ("Check what version has __constructor__ vs constructor");
o = new Object ();
trace (o.__constructor__);
trace (o.hasOwnProperty ("__constructor__"));
trace (o.constructor);
trace (o.hasOwnProperty ("constructor"));

loadMovie ("FSCommand:quit", "");
