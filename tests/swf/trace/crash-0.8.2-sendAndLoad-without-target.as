// makeswf -v 7 -s 200x150 -r 15 -o crash-0.8.2-sendAndLoad-without-target.swf crash-0.8.2-sendAndLoad-without-target.as

l = new LoadVars ();
l.sendAndLoad ("hello", undefined);

getURL ("fscommand:quit", "");
