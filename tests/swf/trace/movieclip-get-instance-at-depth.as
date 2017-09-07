// makeswf -v 7 -r 1 -o test-7.swf test.as

#include "values.as"

trace (this.getInstanceAtDepth ());

this.createEmptyMovieClip ("a", 0);
this.createEmptyMovieClip ("c", 3);
this.createEmptyMovieClip ("d", 6);
this.createEmptyMovieClip ("e", -1);
this.createEmptyMovieClip ("f", -4);

a.createEmptyMovieClip ("a", 0);
a.createEmptyMovieClip ("c", 3);
a.createEmptyMovieClip ("d", 6);
a.createEmptyMovieClip ("e", -1);
a.createEmptyMovieClip ("f", -4);

for (var i = -5; i <= 5; i++) {
  trace ("this " + i + ": " + this.getInstanceAtDepth (i));
  trace ("a " + i + ": " + a.getInstanceAtDepth (i));
}

for (var i = 0; i < values.length; i++) {
  trace ("this " + names[i] + ": " + this.getInstanceAtDepth (values[i]));
}

// test on TextField object
TextField.prototype.getInstanceAtDepth =
  MovieClip.prototype.getInstanceAtDepth;
TextField.prototype.createEmptyMovieClip =
  MovieClip.prototype.createEmptyMovieClip;
this.createTextField ("t", 5, 0, 0, 100, 100);
t.createEmptyMovieClip ("a", 0);

trace ("t 0: " + t.getInstanceAtDepth (0));
trace ("t 1: " + t.getInstanceAtDepth (1));

loadMovie ("FSCommand:quit", "");
