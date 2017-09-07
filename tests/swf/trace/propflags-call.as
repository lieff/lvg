// makeswf -v 7 -r 1 -o propflags-call-7.swf propflags-call.as

var o = {};
o.test = function () {
  trace ("test()");
  return true;
};

trace ("o.test: 0");
trace (o.test ());
for (var i = 0; i <= 13; i++) {
  trace ("o.test: " + (1 << i));
  ASSetPropFlags (o, "test", 1 << i, 0);
  trace (o.test ());
  ASSetPropFlags (o, "test", 0, 1 << i);
}

loadMovie ("FSCommand:quit", "");
