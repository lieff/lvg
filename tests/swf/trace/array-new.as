// makeswf -v 7 -s 200x150 -r 1 -o array-new.swf array-new.as

trace ("Check how \"Array\" and \"new Array\" react to parameters");

#include "values.as"

// check default values first
for (i = 0; i < values.length; i++) {
  trace (Array (values[i]));
  trace (new Array (values[i]));
};
trace (Array (values));
trace (new Array (values));
trace (Array (names));
trace (new Array (names));

// check numbers
trace (Array (42));
trace (new Array (42));
trace (Array (5.5));
trace (new Array (5.5));
trace (Array (-3));
trace (new Array (-3));

loadMovie ("FSCommand:quit", "");
