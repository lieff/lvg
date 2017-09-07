// makeswf -v 7 -s 200x150 -r 1 -o conversion-functions.swf conversion-functions.as

#include "values.as"

for (i = 0; i < values.length; i++) {
  trace (names[i]);
  x = Boolean (values[i]);
  trace ("  Boolean: " + x + " (" + typeof (x) + ")");
  x = int (values[i]);
  trace ("  Integer: " + x + " (" + typeof (x) + ")");
  x = Number (values[i]);
  trace ("  Number: " + x + " (" + typeof (x) + ")");
  x = Object(values[i]);
  trace ("  Object: " + x + " (" + typeof (x) + ")");
}

getURL ("fscommand:quit", "");
