// makeswf -v 7 -r 1 -o propflags-get-7.swf propflags-get.as

var o = { test: true };

trace ("o.test: 0");
trace (o.test);
for (var i = 0; i <= 13; i++) {
  trace ("o.test: " + (1 << i));
  ASSetPropFlags (o, "test", 1 << i, 0);
  trace (o.test);
  ASSetPropFlags (o, "test", 0, 1 << i);
}

loadMovie ("FSCommand:quit", "");

