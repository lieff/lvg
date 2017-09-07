// makeswf -v 7 -s 200x150 -r 1 -o netstream-onstatus-notfound.swf netstream-onstatus-notfound.as

trace ("Test onStatus emission for nonexisting movies");
nc = new NetConnection ();
nc.connect (null);
ns = new NetStream (nc);
ns.onStatus = function (info) {
  trace ("NetStream onStatus called");
  trace (info.code);
  trace (info.level);
};
trace ("Calling play");
ns.play ("doesnotexist.flv");
trace ("done calling play");
