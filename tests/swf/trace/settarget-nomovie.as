// makeswf -v 7 -s 200x150 -r 1 -o settarget-nomovie.swf settarget-nomovie.as

trace ("test how setTarget behaves when called with a non-movie object");

x = 0;
a = { x: 1 };
createEmptyMovieClip ("b", 0);
b.x = 2;
setTarget ("b");
setTarget ("a");
trace (x);
setTarget ("");

loadMovie ("FSCommand:quit", "");
