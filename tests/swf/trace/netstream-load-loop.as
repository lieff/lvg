// makeswf -v 7 -s 200x150 -r 1 -o netstream-load-loop.swf netstream-load-loop.as

nc = new NetConnection ();
nc.connect (null);
ns = new NetStream (nc);
ns.onStatus = function (o) {
  ns.play ("doesnotexist.flv");
  trace ("hey");
};
onEnterFrame = function () {
  trace ("ho");
  x++;
  if (x >= 5)
    loadMovie ("FSCommand:quit", "");
};
ns.play ("doesnotexist.flv");
x = 0;

//loadMovie ("FSCommand:quit", "");

