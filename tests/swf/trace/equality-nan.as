// makeswf -v 7 -s 200x150 -r 1 -o equality-nan.swf equality-nan.as

x = 0/0;
nans = [ 0/0, 0/0, NaN, NaN, Number ("asdf"), "asdf", "asdf", x, x];

for (i = 0; i < nans.length; i++) {
  trace (nans[i]);
  for (j = 0; j < nans.length; j++) {
    trace (i + " == " + j + " => " + (nans[i] == nans[j]));
  }
}


getURL ("fscommand:quit", "");
