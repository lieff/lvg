// makeswf -v 7 -s 200x150 -r 1 -o textfield-x-and-width.swf textfield-x-and-width.as

#include "values.as"

createTextField ("t", 0, 100, 0, 100, 50);

for (i = 0; i < values.length; i++) {
  t._width = 100;
  t._width = values[i];
  trace (Number (values[i]));
  trace (names[i] + ": " + t._width);
};

t._width = 100;
for (i = 0; i < values.length; i++) {
  t._x = 100;
  t._x = values[i];
  trace (Number (values[i]));
  trace (names[i] + ": " + t._x);
};

getURL ("fscommand:quit", "");
