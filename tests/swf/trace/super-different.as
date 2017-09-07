// makeswf -v 7 -s 200x150 -r 1 -o super-different.swf super-different.as

trace ("Check calls to differently named functions on super");
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
  trace ("foo: 1");
};
One.prototype.bla = function () {
  trace ("bla: 1");
};
Two.prototype.foo = function () {
  super.bla ();
  trace ("foo: 2");
};
Three.prototype.bla = function () {
  super.foo ();
  trace ("bla: 3");
};
x = new Four ();
x.foo = function () {
  super.bla ();
  trace ("foo: x");
};
x.bla = function () {
  super.foo ();
  trace ("bla: x");
};
x.foo ();
x.bla ();

loadMovie ("FSCommand:quit", "");
