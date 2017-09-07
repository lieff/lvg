// makeswf -v 7 -r 1 -o instance-of-propflags-7.swf instance-of-propflags.as

var f = { prototype: {} };
var o = { __proto__: f.prototype };

for (var i = 1; i <= 13; i++) {
  trace ("o.__proto__: " + (1 << i));
  ASSetPropFlags (o, "__proto__", 1 << i, 0);
  trace (o instanceOf f);
  ASSetPropFlags (o, "__proto__", 0, 1 << i);
}

/* FIXME currently we check these wrong
for (var i = 1; i <= 13; i++) {
  trace ("f.prototype: " + (1 << i));
  ASSetPropFlags (f, "prototype", 1 << i, 0);
  trace (o instanceOf f);
  ASSetPropFlags (f, "prototype", 0, 1 << i);
}*/

loadMovie ("FSCommand:quit", "");
