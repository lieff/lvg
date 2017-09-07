// makeswf -v 7 -s 200x150 -r 1 -o round-direction.swf round-direction.as

trace ("Check rounding direction");

trace (Math.round (1.5));
trace (Math.round (1));
trace (Math.round (0.5));
trace (Math.round (0));
trace (Math.round (-0.5));
trace (Math.round (-1));
trace (Math.round (-1.5));

loadMovie ("FSCommand:quit", "");
