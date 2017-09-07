// makeswf -v 7 -s 200x150 -r 1 -o setvariable-function-target.swf setvariable-function-target.as

trace ("Check SetVariable inside functions sets variables on the target");

createEmptyMovieClip ("movie", 0);

function foo () {
  setTarget ("movie");
  bla = 42;
  trace (_root.bla);
  trace (_root.movie.bla);
};
foo ();
trace (bla);
trace (movie.bla);

loadMovie ("FSCommand:quit", "");
