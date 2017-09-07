// makeswf -v 7 -s 200x150 -r 1 -o definelocal-target.swf definelocal-target.as

trace ("Check DefineLocal sets variables on the target");

createEmptyMovieClip ("movie", 0);

setTarget ("movie");
var bla = 42;
setTarget ("");
trace (bla);
trace (movie.bla);

loadMovie ("FSCommand:quit", "");
