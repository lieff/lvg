// makeswf -v 7 -s 200x150 -r 1 -o moviecliploader-constructor.swf moviecliploader-constructor.as

trace ("Call MovieClipLoader on a normal object");

o = {};
trace (MovieClipLoader (o));
trace (o._listeners);

loadMovie ("fscommand:QUIT", "");
