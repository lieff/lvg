// makeswf -v 7 -s 200x150 -r 1 -o netstream-fscommand.swf netstream-fscommand.as

trace ("Check if fscommand works for NetStream.play");
nc = new NetConnection ();
nc.connect (null);
ns = new NetStream (nc);
ns.play ("fscommand:quit");
onEnterFrame = function () {
  if (nc) {
    delete nc;
  } else if (ns) {
    delete ns;
  } else {
    loadMovie ("fscommand:quit", "");
  }
};
