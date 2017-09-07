// makeswf -v 7 -s 200x150 -r 1 -o foreach-propflags.swf foreach-propflags.as

var o = { test: true };

trace ("o.test: 0");
var found = false;
for (var prop in o) {
  if (prop == "test")
    found = true;
}
trace (found);
for (var i = 0; i <= 13; i++) {
  trace ("o.test: " + (1 << i));
  ASSetPropFlags (o, "test", 1 << i, 0);
  found = false;
  for (var prop in o) {
    if (prop == "test")
      found = true;
  }
  trace (found);
  ASSetPropFlags (o, "test", 0, 1 << i);
}

loadMovie ("FSCommand:quit", "");

