// makeswf -v 7 -s 200x150 -r 1 -o addProperty.swf addProperty.as

function get () {
  trace ("get");
  return 1;
};
function get2 () {
  trace ("get2");
  return 2;
};
function set (x) {
  trace ("set");
  this.bla = x;
};
o = new Object ();
o.foo = 42;
trace (o.addProperty ("foo", get, null));
trace (o.foo);
trace (o.addProperty ("foo", get2, null));
trace (o.foo);
trace (o.addProperty ("foo", get));
trace (o.foo);
trace (o.addProperty ("foo", get, 10));
trace (o.foo);
trace (o.addProperty ("foo", get, get));
trace (o.foo);
trace (o.addProperty ("foo", get2, undefined));
trace (o.foo);
trace (o.addProperty ("foo", get, set));
trace (o.foo);
o.foo = 42;
trace (o.foo);
trace (o.bla);

loadMovie ("FSCommand:quit", "");
