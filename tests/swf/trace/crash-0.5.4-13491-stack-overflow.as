// makeswf -v 7 -s 200x150 -r 1 -o crash-0.5.4-13491-stack-overflow.swf crash-0.5.4-13491-stack-overflow.as

x = { toString: function () { return String (this); } };

trace (x);
loadMovie ("fscommand:quit", "");
