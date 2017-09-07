// makeswf -v 7 -r 1 -o get-depth-7.swf get-depth.as

// TODO: Button

this.createEmptyMovieClip ("m", 4);
this.createTextField ("t", 5, 0, 0, 100, 100);

m.getDepthMovie = t.getDepthMovie = _global.MovieClip.prototype.getDepth;
m.getDepthText = t.getDepthText = _global.TextField.prototype.getDepth;

trace ("m.getDepthMovie () = " + m.getDepthMovie ());
trace ("m.getDepthText () = " + m.getDepthText ());
trace ("t.getDepthMovie () = " + t.getDepthMovie ());
trace ("t.getDepthText () = " + t.getDepthText ());

loadMovie ("FSCommand:quit", "");
