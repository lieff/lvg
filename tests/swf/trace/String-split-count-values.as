// makeswf -v 7 -s 200x150 -r 1 -o String-split-count-values.swf String-split-count-values.as

#include "values.as"

s = "12345";

for (i = 0; i < values.length; i++) {
  trace (names[i] + " - " + s.split ("23", values[i]));
};

getURL ("fscommand:quit", "");
