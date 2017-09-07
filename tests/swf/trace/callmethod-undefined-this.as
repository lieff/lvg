// makeswf -v 7 -s 200x150 -r 1 -o callmethod-undefined-this.swf callmethod-undefined-this.as

trace ("Test the behaviour of CallMethod with a function name of undefined or \"\"");
function foo () {
  trace (this);
  this.x = 15;
  trace (this.x);
  return this;
};
asm {
  push 0, "foo"
  getvariable
  push undefined
  callmethod
  trace
};
asm {
  push 0, "foo"
  getvariable
  push ""
  callmethod
  trace
};

loadMovie ("FSCommand:quit", "");
