// makeswf -v 7 -r 1 -o crash-0.5.5-cvs-_global-7.swf crash-0.5.5-cvs-_global.as

var x = 1;
this.createTextField ("a", 1, 0, 0, 100, 100);
a.variable = "_global.x";

trace ("No crash?");

loadMovie ("FSCommand:quit", "");
