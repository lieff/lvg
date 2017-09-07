// makeswf -v 7 -s 200x150 -r 1 -o loadSound-matrix-repeat.swf loadSound-matrix-no-target.as

Sound.prototype.toString = function () {
  var tf = this.getTransform ();
  return "{ " + tf.ll + ", " + tf.rl + ", " + tf.lr + ", " + tf.rr + "} (" + this.getVolume () + ", " + this.getPan () + ")";
};

createEmptyMovieClip ("a", 0);
s = new Sound (a);
t = new Sound (a);
s.loadSound ("test.mp3", true);
a.removeMovieClip ();
trace (s);
trace (t);


getURL ("fscommand:quit", "");
