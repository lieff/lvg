// makeswf -v 7 -s 200x150 -r 1 -o sound-textfield.swf sound-textfield.as

Sound.prototype.toString = function () {
  var tf = this.getTransform ();
  return tf + ": { " + tf.ll + ", " + tf.rl + ", " + tf.lr + ", " + tf.rr + "} (" + this.getVolume () + ", " + this.getPan () + ")";
};

x = new Sound ();
x.setPan (-50);
createTextField ("t", 0, 0, 0, 100, 50);
x = new Sound (t);
x.setPan (50);
x = new Sound ();
trace (x);
x = new Sound (t);
trace (x);
x = new Sound ("asdf");
trace (x);

getURL ("fscommand:quit", "");
