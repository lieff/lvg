// makeswf -v 7 -s 200x150 -r 1 -o remove-depths.swf remove-depths.as

depths = [ 0, -16384, -16385, 1000 ];
for (i = 0; i < depths.length; i++) {
  bla = createEmptyMovieClip ("foo" + i, depths[i]);
  bla.depth = bla.getDepth ();
  bla.onUnload = function () {
    trace (this.depth + " => " + this.getDepth ());
  };
  bla.removeMovieClip ();
}
// We print that here so we know the onUnload gets triggered later
trace ("Check how removing movieclips affects depth");

loadMovie ("FSCommand:quit", "");
