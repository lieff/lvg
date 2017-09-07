// makeswf -v 7 -s 200x150 -r 1 -o loadSound-matrix-reset.swf loadSound-matrix-reset.as

Sound.prototype.toString = function () {
  var tf = this.getTransform ();
  return "{ " + tf.ll + ", " + tf.rl + ", " + tf.lr + ", " + tf.rr + "} (" + this.getVolume () + ", " + this.getPan () + ")";
};
stop();

s = new Sound (this);
s.loadSound ("test.mp3", true);
s.setVolume (25);

trace (new Sound (this));
trace (new Sound ());

s = new Sound ();
s.setVolume (25);
s.loadSound ("test.mp3", true);
trace (s);

s = new Sound ();
s.loadSound ("test.mp3", true);
trace (s);

s = new Sound (this);
s.setVolume (25);
s.loadSound ("test.mp3", true);
trace (s);

s = new Sound (this);
s.loadSound ("test.mp3", true);
trace (s);


getURL ("fscommand:quit", "");
