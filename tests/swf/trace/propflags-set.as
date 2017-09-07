// makeswf -v 7 -r 1 -o propflags-set-7.swf propflags-set.as

var o = new Object ();

o[0] = 0;
for (var i = 0; i <= 13; i++) {
  if (i == 11) continue; // flash player bug
  o[1 << i] = 1 << i;
  ASSetPropFlags (o, 1 << i, 1 << i, 0);
}

trace ("0: " + o[0]);
o[0] = "0 reset";
trace ("0: " + o[0]);
ASSetPropFlags (o, "0", 0, 8191);

trace ((1 << i) + ": " + o[1 << i]);
for (var i = 0; i <= 13; i++) {
  if (i == 11) continue; // flash player bug
  trace ((1 << i) + ": " + o[1 << i]);
  o[1 << i] = (1 << i) + " reset";
  trace ((1 << i) + ": " + o[1 << i]);
  ASSetPropFlags (o, (1 << i), 0, 8191);
  trace ((1 << i) + ": " + o[1 << i]);
}

loadMovie ("FSCommand:quit", "");
