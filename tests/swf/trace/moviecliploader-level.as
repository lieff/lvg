// makeswf -v 7 -s 200x150 -r 1 -o moviecliploader-level.swf moviecliploader-level.as

l = new MovieClipLoader ();
l.onLoadStart = function (m) {
  trace ("I win");
};
if (this == _level1) {
  getURL ("fscommand:quit", "");
} else {
  trace ("Your mom");
  trace (l.loadClip ("moviecliploader-level-" + __SWF_VERSION__ + ".swf", "_level1"));
  trace (_level1);
}
