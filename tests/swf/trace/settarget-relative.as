// makeswf -v 7 -s 200x150 -r 1 -o settarget-relative.swf settarget-relative.as

trace ("Check whether multiple setTarget calls are relative to each other");

createEmptyMovieClip ("a", 0);
a.createEmptyMovieClip ("a", 0);

setTarget ("a");
setTarget ("a");
trace (_target);

loadMovie ("FSCommand:quit", "");
