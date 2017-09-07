// makeswf -v 7 -s 200x150 -r 1 -o movie62.swf movie62.as

#include "values.as"

trace ("check Equals and StrictEquals action");
for (i = 0; i < values.length; i++) {
  for (j = 0; j < values.length; j++) {
    trace ("---");
    trace (names[i] + " == " + names[j] + " = " + (values[i] == values[j]));
#if __SWF_VERSION__ > 5
    trace (names[i] + " === " + names[j] + " = " + (values[i] === values[j]));
#endif
  };
};

loadMovie ("FSCommand:quit", "");
