// makeswf -v 7 -s 200x150 -r 1 -o prototype-recursion-get.swf prototype-recursion-get.as

trace ("test recursion limit of getting properties");
o = new Object ();
o.x = 42;
for (i = 0; ; i++) {
  p = new Object ();
  p.__proto__ = o;
  o = p;
  trace (">>>");
  trace (i + ": " + o.x);
}

loadMovie ("FSCommand:quit", "");
