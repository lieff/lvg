// makeswf -v 7 -s 200x150 -r 1 -o prototype-recursion-addProperty.swf prototype-recursion-addProperty.as

o = new Object ();
o.addProperty ("x", function () {}, null);
for (i = 0; ; i++) {
  p = new Object ();
  p.hasOwnProperty = o.hasOwnProperty;
  p.__proto__ = o;
  o = p;
  o.x = 42;
  trace (i + ": " + o.hasOwnProperty ("x"));
};

loadMovie ("FSCommand:quit", "");
