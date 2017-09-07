// makeswf -v 7 -s 200x150 -r 1 -o crash-0.6.6-xmlsocket.swf crash-0.6.6-xmlsocket.as

x = new XMLSocket ();
x.connect ("localhost", 80);

y = new XMLSocket ();
y.send ("hello");

getURL ("fscommand:quit", "");
