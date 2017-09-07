// makeswf -v 7 -s 200x150 -r 1 -o loadvars-decode.swf loadvars-decode.as

var l = new LoadVars ();
trace (l.decode ("a=b&c=d"));
trace (l.decode ());
trace (l.decode (""));
trace (l.decode (undefined));
trace (l.decode ("a=b&c=d", "e=f"));

var o = {};
o.decode = ASnative (301, 3);
trace (o.decode ("a=b&c=d"));
trace (o.decode ());

createEmptyMovieClip ("a", 0);
a.decode = ASnative (301, 3);
trace (a.decode ("a=b&c=d"));
trace (a.decode ());

getURL ("fscommand:quit", "");
