// makeswf -v 7 -s 200x150 -r 1 -o loadSound-matrix-repeat.swf loadSound-matrix-repeat.as

Sound.prototype.toString = function () {
  var tf = this.getTransform ();
  return "{ " + tf.ll + ", " + tf.rl + ", " + tf.lr + ", " + tf.rr + "} (" + this.getVolume () + ", " + this.getPan () + ")";
};
stop();

s = new Sound ();
s.loadSound ("test.mp3", true);
s.setVolume (25);
trace (s);
s.loadSound ("test.mp3", true);
trace (s);


getURL ("fscommand:quit", "");
