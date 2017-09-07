// makeswf -v 7 -s 200x150 -r 1 -o setvariable-callback.swf setvariable-callback.as

trace ("Check SetVariable in callbacks sets variables on the target at function definition time");

createEmptyMovieClip ("movie", 0);

movie.onEnterFrame = function () {
  bla = 42;
  trace (this.bla);
  trace (_root.bla);
  trace (_root.movie.bla);

  loadMovie ("FSCommand:quit", "");
};
