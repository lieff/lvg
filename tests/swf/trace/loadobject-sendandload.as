// makeswf -v 7 -s 200x150 -r 15 -o loadobject-sendandload-7.swf loadobject-sendandload.as

var o = {};
o.sendAndLoad = ASnative (301, 2);
var t = { test: "supernormal" };
o.test = "verynormal";

t.onData2 = function (str) {
  trace ("Finished: " + str.split("\n").join("\\n"));

  getURL ("fscommand:quit");
};

t.onData = function (str) {
  trace ("Finished: " + str.split("\n").join("\\n"));

  t.onData = t.onData2;

  o.sendAndLoad("loadvars.txt?test=paranormal", t, "POST");
};

o.sendAndLoad("loadvars.txt?test=paranormal", t, "GET");
