// makeswf -v 7 -s 200x150 -r 15 -o movieclip-hittest-parent-8.swf movieclip-hittest-parent-8.as

this.createEmptyMovieClip ("a", 0);

with (a) {
  _x = 10;
  _y = 10;
  beginFill (0xFF0000);
  moveTo (0, 0);
  lineTo (25, 0);
  lineTo (25, 25);
  lineTo (0, 25);
  lineTo (0, 0);
  endFill ();
}

a.createEmptyMovieClip ("b", 0);
with (a.b) {
  _x = 20;
  _y = 20;
  beginFill (0x00FF00);
  moveTo (0, 0);
  lineTo (25, 0);
  lineTo (25, 25);
  lineTo (0, 25);
  lineTo (0, 0);
  endFill ();
}

a.b.createEmptyMovieClip ("c", 0);
with (a.b.c) {
  _x = 20;
  _y = 20;
  beginFill (0x0000FF);
  moveTo (0, 0);
  lineTo (25, 0);
  lineTo (25, 25);
  lineTo (0, 25);
  lineTo (0, 0);
  endFill ();
}

this.createEmptyMovieClip ("d", 1);

with (d) {
  _x = 10;
  _y = 10;
  beginFill (0xCCCCCC);
  moveTo (10, 10);
  lineTo (35, 10);
  lineTo (35, 35);
  lineTo (10, 35);
  lineTo (10, 10);
  endFill ();
}

trace (a.hitTest (5, 5, true));
trace (a.hitTest (10, 10, true));
trace (a.hitTest (15, 15, true));
trace (a.hitTest (5, 5, false));
trace (a.hitTest (10, 10, false));
trace (a.hitTest (15, 15, false));

trace (a.b.hitTest (25, 25, true));
trace (a.b.hitTest (30, 30, true));
trace (a.b.hitTest (35, 35, true));
trace (a.b.hitTest (25, 25, false));
trace (a.b.hitTest (30, 30, false));
trace (a.b.hitTest (35, 35, false));

trace (a.b.c.hitTest (45, 45, true));
trace (a.b.c.hitTest (50, 50, true));
trace (a.b.c.hitTest (55, 55, true));
trace (a.b.c.hitTest (45, 45, false));
trace (a.b.c.hitTest (50, 50, false));
trace (a.b.c.hitTest (55, 55, false));

trace (a.hitTest (a));
trace (a.hitTest (a.b));
trace (a.hitTest (a.b.c));
trace (a.hitTest (d));
trace (a.b.hitTest (a));
trace (a.b.hitTest (a.b));
trace (a.b.hitTest (a.b.c));
trace (a.b.hitTest (d));
trace (a.b.c.hitTest (a));
trace (a.b.c.hitTest (a.b));
trace (a.b.c.hitTest (a.b.c));
trace (a.b.c.hitTest (d));
trace (d.hitTest (a));
trace (d.hitTest (a.b));
trace (d.hitTest (a.b.c));
trace (d.hitTest (d));

getURL ("fscommand:quit", "");
