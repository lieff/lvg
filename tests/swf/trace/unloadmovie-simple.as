// makeswf -v 7 -s 200x150 -r 1 -o unloadmovie-simple.swf unloadmovie-simple.as

x = createEmptyMovieClip ("a", 1);
loadMovie ("", a); 
a.x = 42;
a.createEmptyMovieClip ("a", 1);

onEnterFrame = function () {
  trace (a);
  trace (a.x);
  trace (a.a);
  trace (a == x);
  if (!a.a)
    loadMovie ("fscommand:QUIT", "");
};
onEnterFrame ();
