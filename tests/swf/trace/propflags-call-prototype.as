// makeswf -v 7 -r 1 -o propflags-call-prototype-7.swf propflags-call-prototype.as

var p = {};
p.test = function () {
  trace ("test()");
  return true;
};
var o = { __proto__: p };

trace ("o.__proto__: 0");
trace (o.test ());
for (var i = 0; i <= 13; i++) {
  trace ("o.__proto__: " + (1 << i));
  ASSetPropFlags (o, "__proto__", 1 << i, 0);
  trace (o.test ());
  ASSetPropFlags (o, "__proto__", 0, 1 << i);
}

loadMovie ("FSCommand:quit", "");
