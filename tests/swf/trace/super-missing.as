// makeswf -v 7 -s 200x150 -r 1 -o super-missing.swf super-missing.as

trace ("Check how functions that aren't overwritten are handled when called via super");

function One () {
  trace (1);
};
function Two () {
  super ();
  trace (2);
};
function Three () {
  super ();
  trace (3);
};
function Four () {
  super ();
  trace (4);
};
asm {
  push "Two"
  getvariable
  push "One"
  getvariable
  extends
};
asm {
  push "Three"
  getvariable
  push "Two"
  getvariable
  extends
};
asm {
  push "Four"
  getvariable
  push "Three"
  getvariable
  extends
};
One.prototype.foo = function () {
  super.foo ();
  trace ("foo: 1");
};
Four.prototype.foo = function () {
  super.foo ();
  trace ("foo: 4");
};
x = new Four ();
x.foo ();

loadMovie ("FSCommand:quit", "");
