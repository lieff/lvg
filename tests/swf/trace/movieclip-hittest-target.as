// makeswf -v 7 -s 200x150 -r 15 -o movieclip-hittest-target-8.swf movieclip-hittest-target-8.as

function fill (movie) {
  with (movie) {
    beginFill (0xFF0000);
    moveTo (10, 10);
    lineTo (50, 10);
    lineTo (50, 50);
    lineTo (10, 50);
    lineTo (10, 10);
    endFill ();
  }
}

this.createEmptyMovieClip ("a", 0);
fill (a);
this.createEmptyMovieClip ("b", 1);
fill (b);

trace ("Test whether target parameter can be a string");
trace (a.hitTest (b));
trace (a.hitTest ("b"));

getURL ("fscommand:quit", "");
