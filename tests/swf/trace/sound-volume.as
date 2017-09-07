// makeswf -v 7 -s 200x150 -r 1 -o sound-volume.swf sound-volume.as

Sound.prototype.toString = function () {
  var tf = this.getTransform ();
  return "{ " + tf.ll + ", " + tf.rl + ", " + tf.lr + ", " + tf.rr + "} (" + this.getVolume () + ", " + this.getPan () + ")";
};
stop();

s = new Sound ("a");
s.setVolume (50);
trace ("from nonexistant name: " + s);
createEmptyMovieClip ("a", 0);
trace ("after creating it: " + s);
s = new Sound (a);
trace ("new movie with same name: " + s);
a._name = "b";
trace ("renaming that movie: " + s);
b.removeMovieClip ();
trace ("after removing the movie: " + s);

s.loadSound ("test.mp3", true);
trace ("after loadSound: " + s);
s.setVolume (25);
trace (s);

trace (new Sound (a));
setTarget ("a");
trace (new Sound ());
setTarget ("");
trace (new Sound (this));

getURL ("fscommand:quit", "");
