// makeswf -v 7 -r 1 -o movieclip-get-swf-version-object-7.swf movieclip-get-swf-version-object.as

// try running getSWFVersion on some other objects

// no object
var func = this.getSWFVersion;
asm {
  push 0, "func"
  getvariable
  push undefined
  callmethod
  trace
};

var n = new Number (42);
n.func = func;
trace (n.func ());

var f = new TextField ();
f.func = func;
trace (f.func ());

this.createTextField ("t", 0, 0, 0, 100, 100);
t.func = func;
trace (t.func ());

loadMovie ("FSCommand:quit", "");
