// makeswf -v 7 -r 1 -o remove-movie-7.swf remove-movie.as

// TODO: Button

this.createEmptyMovieClip ("m", 4);
this.createTextField ("t", 5, 0, 0, 100, 100);

m.test = "Wasn't removed";
t.test = "Wasn't removed";

// work around ming lowercasing removeMovieClip
t["removeMovieClip"] = _global.MovieClip.prototype["removeMovieClip"];
m.removeTextField = _global.TextField.prototype.removeTextField;

trace ("MovieClip with removeTextField:");
trace (m.removeTextField ());
trace (m.test);

trace ("TextField with removeMovieClip:");
trace (t["removeMovieClip"] ());
trace (t.test);

this.createEmptyMovieClip ("m", 4);
this.createTextField ("t", 5, 0, 0, 100, 100);

m.test = "Wasn't removed";
t.test = "Wasn't removed";

trace ("MovieClip with removeMovieClip:");
trace (m["removeMovieClip"] ());
trace (m.test);

trace ("TextField with removeTextField:");
trace (t.removeTextField ());
trace (t.test);

loadMovie ("FSCommand:quit", "");
