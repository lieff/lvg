// makeswf -v 7 -r 1 -o propflags-set-prototype-7.swf propflags-set-prototype.as

function native_get () {
  trace ("native_get");
  return "p";
}

function native_set () {
  trace ("native_set: " + arguments);
}

var p = { normal: "p" };
p.addProperty ("native", native_get, native_set);
var o = { __proto__: p };

trace ("o.__proto__: 0");
trace (o.normal);
trace (o.native);
trace (p.normal);
trace (p.native);
o.normal = "o";
o.native = "o";
trace (o.normal);
trace (o.native);
trace (p.normal);
trace (p.native);

for (var i = 0; i <= 13; i++) {
  trace ("o.__proto__: " + (1 << i));
  ASSetPropFlags (a, "__proto__", 1 << i, 0);
  trace (o.normal);
  trace (o.native);
  trace (p.normal);
  trace (p.native);
  o.normal = "o";
  o.native = "o";
  trace (o.normal);
  trace (o.native);
  trace (p.normal);
  trace (p.native);
  ASSetPropFlags (a, "__proto__", 0, 1 << i);
  delete o.normal;
  delete o.native;
  p.normal = "p";
}

loadMovie ("FSCommand:quit", "");
