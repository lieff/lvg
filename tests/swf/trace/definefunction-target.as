// makeswf -v 7 -s 200x150 -r 1 -o definefunction-target.swf definefunction-target.as

trace ("Check which target DefineFunction uses");

createEmptyMovieClip ("a", 0);
setTarget ("a");
function foo () {
  trace (_target);
};
foo = function () {
  trace (_target);
};
setTarget ("");
foo ();
a.foo ();

loadMovie ("FSCommand:quit", "");
