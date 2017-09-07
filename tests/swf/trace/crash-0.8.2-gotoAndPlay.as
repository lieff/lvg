// makeswf -v 7 -s 200x150 -r 1 -o crash-0.8.2-gotoAndPlay.swf crash-0.8.2-gotoAndPlay.as

createEmptyMovieClip ("a", 0);
a.gotoAndPlay ("hi");
a.gotoAndStop ("ho");

getURL ("fscommand:quit", "");
