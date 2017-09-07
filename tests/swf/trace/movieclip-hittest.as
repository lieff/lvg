// makeswf -v 7 -s 200x150 -r 15 -o movieclip-hittest-8.swf movieclip-hittest-8.as

this.createEmptyMovieClip ("a", 0);

with (a) {
  beginFill (0xFF0000);
  moveTo (10, 10);
  lineTo (50, 10);
  lineTo (50, 50);
  lineTo (10, 50);
  lineTo (10, 10);
  endFill ();

  trace (hitTest (5, 5, true));
  trace (hitTest (10, 10, true));
  trace (hitTest (15, 15, true));
  trace (hitTest (5, 5, false));
  trace (hitTest (10, 10, false));
  trace (hitTest (15, 15, false));
}

this.createEmptyMovieClip ("b", 1);
with (b) {
  beginFill (0x00FF00);
  moveTo (30, 30);
  lineTo (70, 30);
  lineTo (70, 70);
  lineTo (30, 70);
  lineTo (30, 30);
  endFill ();
}

this.createEmptyMovieClip ("c", 2);
with (c) {
  beginFill (0x0000FF);
  moveTo (60, 60);
  lineTo (100, 60);
  lineTo (100, 100);
  lineTo (60, 100);
  lineTo (60, 60);
  endFill ();
}

trace (a.hitTest (a));
trace (a.hitTest (b));
trace (a.hitTest (c));
trace (b.hitTest (a));
trace (b.hitTest (b));
trace (b.hitTest (c));
trace (c.hitTest (a));
trace (c.hitTest (b));
trace (c.hitTest (c));

getURL ("fscommand:quit", "");
