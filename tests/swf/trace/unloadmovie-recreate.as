// makeswf -v 7 -s 200x150 -r 1 -o unloadmovie-recreate.swf unloadmovie-recreate.as

x = createEmptyMovieClip ("b", 1);
b._name = "a";

x.loadMovie ("");

onEnterFrame = function () {
  trace (x == a);
  if (x != a)
    loadMovie ("fscommand:quit", "");
};
onEnterFrame ();
