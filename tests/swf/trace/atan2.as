// makeswf -v 7 -s 200x150 -r 1 -o atan2.swf atan2.as

trace(Math.atan2(10, 10));
trace(Math.atan2(10, 0));
trace(Math.atan2(0, 10));
trace(Math.atan2(0, 0));
trace(Math.atan2(0, -10));
trace(Math.atan2(-10, 10));
trace(Math.atan2(-10, 0));
trace(Math.atan2(-10, -10));

loadMovie ("FSCommand:quit", "");
