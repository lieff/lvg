// makeswf -v 7 -r 1 -o rectangle-equals-7.swf rectangle-equals.as

// enable flash structure for version < 8 too for this test
ASSetPropFlags (_global, "flash", 0, 5248);

var Rectangle = flash.geom.Rectangle;

var rect = new Rectangle (10, 20, 30, 40);

trace (rect.equals (rect));
trace (rect.equals (rect.clone ()));
trace (rect.equals (new Rectangle ()));
trace (rect.equals (new Rectangle (10, 20, 30, 40)));
trace (rect.equals (new Rectangle (10, 20, 30, 40.0)));
trace (rect.equals (new Rectangle (10, 20, 30, 40.1)));

var tmp = new Rectangle (10, 20, 30, 40);
tmp.__proto__ = null;
trace (rect.equals (tmp));

loadMovie ("FSCommand:quit", "");
