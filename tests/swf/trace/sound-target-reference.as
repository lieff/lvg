// makeswf -v 7 -s 200x150 -r 1 -o sound-target-reference.swf sound-target-reference.as

createEmptyMovieClip ("a", 0);
if (this == _root) {
  Sound.prototype.toString = function () {
    return "" + this.getVolume ();
  };
  Sound.prototype.toString2 = Sound.prototype.toString;
  a.loadMovie (_url);
  mult = 2;
} else {
  Sound.prototype.toString = function () {
    return this.toString2 () + " - " + this.getVolume ();
  };
  getURL ("fscommand:quit", "");
  mult = 1;
}

s = new Sound ();
s.setVolume (mult * 3);
s = new Sound (a);
s.setVolume (mult * 5);
s = new Sound (this);
s.setVolume (mult * 7);

trace (new Sound (a));
setTarget ("a");
trace (new Sound ());
trace (new Sound ("a"));
trace (new Sound ("/a"));
setTarget ("");
trace (new Sound (this));
