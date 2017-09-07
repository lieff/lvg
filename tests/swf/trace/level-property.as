// makeswf -v 7 -s 200x150 -r 1 -o level-property.swf level-property.as

trace (_level0);
_level0 = "hi";
trace (_level0);
delete _level0;
trace (_level0);
_global._level0 = "hi";
trace (_level0);
this._level0 = "hi";
trace (_level0);
delete this._level0;
trace (_level0);
x = createEmptyMovieClip ("_level0", 0);
trace (_level0);
trace (x);

loadMovie ("FSCommand:quit", "");
