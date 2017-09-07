// makeswf -v 7 -s 200x150 -r 1 -o with-prototypes.swf with-prototypes.as

trace ("Check prototypes get used in With");
x = 0;
_global.x = -1;
o = new Object ();
o.__proto__ = { x: 1 };
with (o) {
  trace (x);
  o.x = 2;
  trace (x);
};

loadMovie ("FSCommand:quit", "");
