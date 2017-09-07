// makeswf -v 7 -s 200x150 -r 1 -o tointeger-numbers.swf tointeger-numbers.as

#include "values.as"

trace ("check ToInteger for various values");

for (i = 0; i < numbers.length; i++) {
  trace (names[i] + " ==int=> " + int (values[i]));
};

loadMovie ("FSCommand:quit", "");
