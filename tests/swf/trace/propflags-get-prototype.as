// makeswf -v 7 -r 1 -o propflags-get-prototype-7.swf propflags-get-prototype.as

function native_get () {
  trace ("native_get");
  return true;
}

var p = { normal: true };
p.addProperty ("native", native_get, null);
var o = { __proto__: p };

trace ("o.__proto__: 0");
trace (o.normal);
trace (o.native);
for (var i = 0; i <= 13; i++) {
  trace ("o.__proto__: " + (1 << i));
  ASSetPropFlags (o, "__proto__", 1 << i, 0);
  trace (o.normal);
  trace (o.native);
  ASSetPropFlags (o, "__proto__", 0, 1 << i);
}

loadMovie ("FSCommand:quit", "");
