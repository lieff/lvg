// makeswf -v 7 -s 200x150 -r 1 -o super-calls.swf super-calls.as

trace ("Check various behaviours of the super object");

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
#if __SWF_VERSION__ <= 5
  Two.prototype = new Object();
  Two.prototype.__proto__ = One.prototype;
  Two.prototype.__constructor__ = One;
  Three.prototype = new Object();
  Three.prototype.__proto__ = Two.prototype;
  Three.prototype.__constructor__ = Two;
#else
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
#endif
One.prototype.foo = function () {
  trace ("foo: 1");
};
Two.prototype.foo = function () {
  super.foo ();
  trace ("foo: 2");
};
Three.prototype.foo = function () {
  super.foo (this);
  trace ("foo: 3");
};
x = new Three ();
x.foo ();
x.foo.call (this);
Three.prototype.foo ();

loadMovie ("FSCommand:quit", "");
