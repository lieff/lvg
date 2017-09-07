// makeswf -v 7 -s 200x150 -r 1 -o add2.swf add2.as

#include "values.as"
attachCallbacks ();

trace ("check Add2 action");
names = new Array();
for (i = 0; i < values.length; i++) {
  names[i] = "(" + i + ") " + values[i];
};

for (i = 0; i < values.length; i++) {
  for (j = 0; j < values.length; j++) {
    trace ("---");
    trace (names[i] + " + " +  names[j]);
    x = values[i] + values[j];
    trace ("---");
    trace (x + " (" + typeof (x) + ")");
  };
};

loadMovie ("FSCommand:quit", "");
