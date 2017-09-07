// makeswf -v 7 -r 1 -o object-isprototypeof-propflags-7.swf object-isprototypeof-propflags.as

var p = {};
var o = { __proto__: p };

trace ("o.__proto__: 0");
trace (p.isPrototypeOf (o));

for (var i = 0; i <= 13; i++) {
  trace ("o.__proto__: " + (1 << i));
  ASSetPropFlags (o, "__proto__", 1 << i, 0);
  trace (p.isPrototypeOf (o));
  ASSetPropFlags (o, "__proto__", 0, 1 << i);
}

loadMovie ("FSCommand:quit", "");
