// makeswf -v 7 -s 200x150 -r 1 -o moviecliploader-unknown-format.swf moviecliploader-unknown-format.as

x = createEmptyMovieClip ("b", 1);
b._name = "a";

l = new MovieClipLoader ();
l.onLoadError = function (m, e, s) {
  trace (m);
  trace (e);
  trace (s);
};
l.onLoadStart = function (m) {
  trace ("start");
};
l.onLoadProgress = function (m, l, t) {
  trace ("that's progress: " + l + "/" + t);
  trace ("that's progress: " + a.getBytesLoaded () + "/" + a.getBytesTotal ());
};
l.onLoadComplete = function (m, l, t) {
  trace ("done \\o/");
};

l.onLoadInit = function (m) {
  trace (a == x);
  trace (a == m);
  trace (a._url.substr (-11));
  a.onEnterFrame = function () { trace ("onEnterFrame"); };
};
l.loadClip ("moviecliploader-unknown-format.as", a);

setTimeout (function () { 
  trace (x == a);
  trace (a._framesloaded + "/" + a._totalframes);
  trace (a.getBytesLoaded () + "/" + a.getBytesTotal ());
  loadMovie ("fscommand:quit", ""); 
}, 5000);
