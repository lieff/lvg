// makeswf -v 7 -s 200x150 -r 1 -o if.swf if.as

#include "values.as"

trace ("check If action");
for (i = 0; i < values.length; i++) {
  if (values[i]) {
    trace (names[i] + ": TRUE");
  } else {
    trace (names[i] + ": FALSE");
  }
};

loadMovie ("FSCommand:quit", "");
