// makeswf -v 7 -r 1 -o super-property-7.swf super-property.as

Bar = function () {
  trace (this.x);
  this.x = 3;
  trace (this.x);
};

Foo = function () {
  trace (this.x);
  this.x = 2;
  trace (this.x);
  super ();
  trace (this.x);
};

Bar.prototype.x = 1;

asm {
  push "Foo"
  getvariable
  push "Bar"
  getvariable
  extends
};

var a = new Foo ();
trace (a.x);

loadMovie ("FSCommand:quit", "");
