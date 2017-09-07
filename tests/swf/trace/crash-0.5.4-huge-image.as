// makeswf -v 7 -s 200x150 -r 1 -o crash-0.5.4-huge-image.swf crash-0.5.4-huge-image.as

l = new MovieClipLoader ();
createEmptyMovieClip ("a", 0);
l.loadClip ("swfdec-huge.jpg", a);
l.onLoadInit = function () {
  loadMovie ("fscommand:quit", "");
};
