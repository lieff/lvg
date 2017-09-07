// makeswf -v 7 -s 200x150 -r 1 -o definelocal-function-target.swf definelocal-function-target.as

trace ("Check DefineLocal in functions sets variables in the function's scope");

createEmptyMovieClip ("movie", 0);

function foo () {
  setTarget ("movie");
  var bla = 42;
  setTarget ("");
  trace (this.bla);
  trace (movie.bla);
}
foo ();
trace (bla);
trace (movie.bla);

loadMovie ("FSCommand:quit", "");
