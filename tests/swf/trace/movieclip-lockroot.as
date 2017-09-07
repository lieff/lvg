// makeswf -v 7 -r 1 -o crash-0.6.0-moviecliploader.swf crash-0.6.0-moviecliploader.as

this.createEmptyMovieClip ("a", 0);
var b = a.createEmptyMovieClip ("b", 1);
this.value = "parent";
a.value = "a";
b.value = "b";

trace (b._root.value);
a._lockroot = true;
trace (b._root.value);
b._lockroot = true;
trace (b._root.value);

getURL ("FSCommand:quit", "");
