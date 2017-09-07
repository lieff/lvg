// makeswf -v 7 -s 200x150 -r 1 -o rotation2.swf rotation2.as

createEmptyMovieClip ("foo", 0);
trace (foo._rotation);
foo._rotation = NaN;
trace (foo._rotation);
foo._rotation = Infinity;
trace (foo._rotation);
foo._rotation = -Infinity;
trace (foo._rotation);
foo._rotation = 12345467890;
trace (foo._rotation);
foo._rotation = Math.PI;
trace (foo._rotation);
foo._rotation = -360;
trace (foo._rotation);
foo._rotation = 360;
trace (foo._rotation);
foo._rotation = 181;
trace (foo._rotation);
foo._rotation = -181;
trace (foo._rotation);
foo._rotation = 180;
trace (foo._rotation);
foo._rotation = -180;
trace (foo._rotation);
foo._rotation = NaN;
trace (foo._rotation);

loadMovie ("FSCommand:quit", "");
