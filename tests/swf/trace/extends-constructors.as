// makeswf -v 7 -s 200x150 -r 1 -o extends-constructors.swf extends-constructors.as

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
  trace (super);
  trace ("foo: 1");
};
Two.prototype.foo = function () {
  super.foo ();
  trace ("foo: 2");
};
Four.prototype.foo = function () {
  super.foo ();
  trace ("foo: 4");
};
function check_name (fun) {
  if (fun === One)
    trace ("One");
  else if (fun === Two)
    trace ("Two");
  else if (fun === Three)
    trace ("Three");
  else if (fun === Four)
    trace ("Four");
  else
    trace (fun);
};
check_name (One.prototype.constructor);
check_name (One.prototype.__constructor__);
check_name (Two.prototype.constructor);
check_name (Two.prototype.__constructor__);
check_name (Three.prototype.constructor);
check_name (Three.prototype.__constructor__);
check_name (Four.prototype.constructor);
check_name (Four.prototype.__constructor__);

loadMovie ("FSCommand:quit", "");
