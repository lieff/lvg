// makeswf -v 7 -s 200x150 -r 1 -o foreach-propflags-prototype.swf foreach-propflags-prototype.as

var p = { test: true };
var o = { __proto__: p };

trace ("o.__proto__: 0");
var found = false;
for (var prop in o) {
  if (prop == "test")
    found = true;
}
trace (found);
for (var i = 0; i <= 13; i++) {
  trace ("o.__proto__: " + (1 << i));
  ASSetPropFlags (o, "__proto__", 1 << i, 0);
  found = false;
  for (var prop in o) {
    if (prop == "test")
      found = true;
  }
  trace (found);
  ASSetPropFlags (o, "__proto__", 0, 1 << i);
}

loadMovie ("FSCommand:quit", "");
