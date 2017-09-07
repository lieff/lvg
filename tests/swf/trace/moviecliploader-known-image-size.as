// makeswf -v 7 -s 200x150 -r 1 -o moviecliploader-known-image-size.swf moviecliploader-known-image-size.as

trace ("Check that image size is known in onLoadInit");
createEmptyMovieClip ("a", 1);
l = new MovieClipLoader ();
l.onLoadInit = function (m) {
  trace (m._width + " / " + m._height);
  loadMovie ("fscommand:quit", "");
};
l.loadClip ("swfdec.jpg", "a");
