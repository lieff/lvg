// makeswf -v 7 -s 200x150 -r 15 -o crash-0.6.6-create-object.swf crash-0.6.6-create-object.as

function Evil () {}
Evil.__constructor__ = Evil;
Evil.prototype = Evil;
var e = new Evil ();

trace ("Created: " + e);

getURL ("fscommand:quit", "");
