// makeswf -v 7 -s 200x150 -r 1 -o moviecliploader-getprogress.swf moviecliploader-getprogress.as

loader = new MovieClipLoader ();
function check (x) {
  p = loader.getProgress (x);
  trace ("Checking: " + x + " (" + typeof (x) + ")");
  trace (p);
  trace (p.bytesLoaded);
  trace (p.bytesTotal);
};

check (this);
check ();
createEmptyMovieClip ("a", 0);
check (a);
check ("/a");

p.getProgress = loader.getProgress;
p = p.getProgress (this);
trace (p);
trace (p.bytesLoaded);
trace (p.bytesTotal);

loadMovie ("fscommand:QUIT", "");
