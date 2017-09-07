// makeswf -v 7 -s 200x150 -r 1 -o crash-0.5.3-divide-by-zero.swf crash-0.5.3-divide-by-zero.as

trace (1 / 0);

loadMovie ("fscommand:QUIT", "");
