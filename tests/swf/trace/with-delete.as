// makeswf -v 7 -s 200x150 -r 1 -o with-delete.swf with-delete.as

trace ("Check that delete inside a With doesn't delete from prototypes.");
x = 0;
o = { x: 1 };
o.__proto__ = { x: 2 };
with (o) {
  trace (x);
  trace (this.x);
  trace (o.x);
  trace (o.__proto__.x);
  delete x;
  trace (x);
  trace (this.x);
  trace (o.x);
  trace (o.__proto__.x);
  delete x;
  trace (x);
  trace (this.x);
  trace (o.x);
  trace (o.__proto__.x);
};

loadMovie ("FSCommand:quit", "");
