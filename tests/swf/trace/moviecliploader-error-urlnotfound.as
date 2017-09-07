// makeswf -v 7 -s 200x150 -r 1 -o moviecliploader-error-urlnotfound.swf moviecliploader-error-urlnotfound.as

x = createEmptyMovieClip ("a", 0);
l = new MovieClipLoader ();
l.onLoadError = function (m, error, status) {
  trace ("onLoadError: " + error + " (" + status + ")");
  loadMovie ("fscommand:quit", "");
};
l.loadClip ("404", "a");

onEnterFrame = function () {
  trace ("enterFrame: " + (x == a ? "old movie" : "new movie"));
};

setTimeout (function () { loadMovie ("fscommand:quit", ""); }, 5000);
trace ("setup done");
