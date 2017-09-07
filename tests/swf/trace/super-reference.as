// makeswf -v 7 -s 200x150 -r 1 -o super-reference.swf super-reference.as

trace ("Check which object super refers to");

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
function check (x) {
  if (x.foo === One.prototype.foo)
    trace (1);
  else if (x.foo === Two.prototype.foo)
    trace (2);
  else if (x.foo === Three.prototype.foo)
    trace (3);
  else if (x.foo === Four.prototype.foo)
    trace (4);
  else
    trace ("5+");    
};
One.prototype.foo = function () {
  trace ("foo: 1");
};
Two.prototype.foo = function () {
  super.foo ();
  trace ("foo: 2");
};
Three.prototype.foo = function () {
  super.foo ();
  trace ("foo: 3");
};
Four.prototype.foo = function () {
  super.foo ();
  trace ("foo: 4");
};
Two.prototype.bla = function () {
  super.foo ();
  trace ("blabla");
  check (super);
};
Three.prototype.bla = function () {
  super.foo ();
  trace ("bla");
  check (super);
};
x = new Four ();
x.foo = function () {
  super.foo ();
  trace ("foo: 5");
  check (super);
};
x.bla ();
x.bla.call (x);
x.foo ();
x.foo.call (x);

loadMovie ("FSCommand:quit", "");
