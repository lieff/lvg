// makeswf -v 7 -r 1 -o loadobject-7.swf loadobject.as

var obj = new Object ();

obj.load = ASnative (301, 0);

obj.onData = function (str) {
  trace ("Got: " + typeof (str) + " '" + str + "'");
  getURL ("FSCommand:quit", "");
};

trace (obj.load ("loadvars.txt%00foobar"));
