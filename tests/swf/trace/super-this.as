// makeswf -v 7 -s 200x150 -r 1 -o super-this.swf super-this.as

trace ("Check super when calling functions on various objects");

function foo () {
  trace (super);
};
x = new foo ();
foo ();
x.foo = foo;
x.foo ();
XML.foo = foo;
XML.foo ();
Function.foo = foo;
Function.foo ();
s = new String ("hi");
s.foo = foo;
s.foo ();
o = new Object ();
o.toString = function () {
#if __SWF_VERSION__ <= 5
  if (o == this)
#else
  if (o === this)
#endif
    trace ("Wohoo");
  return "hi";
};
o.foo = foo;
o.foo ();
o.__proto__ = new Object ();
o.__proto__.__proto__.toString = o.toString;
o.foo ();

loadMovie ("FSCommand:quit", "");
