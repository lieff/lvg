// makeswf -v 7 -s 200x150 -r 1 -o sound-set.swf sound-set.as

Sound.prototype.toString = function () {
  var tf = this.getTransform ();
  return "{ " + tf.ll + ", " + tf.rl + ", " + tf.lr + ", " + tf.rr + "} (" + this.getVolume () + ", " + this.getPan () + ")";
};

values = [ undefined, NaN, Infinity, -1234567890, 1234567890, -20, 0, 20, 40, 60, 80, 100, 120];

for (i = 0; i < values.length; i++) {
  createEmptyMovieClip ("a", 0);
  x = new Sound (a);
  x.setTransform ({ ll:values[i], lr:values[i], rl:values[i], rr:values[i] });
  trace (x);
};

for (i = 0; i < values.length; i++) {
  createEmptyMovieClip ("a", 0);
  x = new Sound (a);
  x.setVolume (values[i]);
  trace (x);
};

for (i = 0; i < values.length; i++) {
  createEmptyMovieClip ("a", 0);
  x = new Sound (a);
  x.setTransform ({ ll: random(101), lr: random (101), rl: random (101), rr: random (101) });
  x.setPan (values[i]);
  trace (x);
};

getURL ("fscommand:quit", "");
