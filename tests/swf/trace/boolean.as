// makeswf -v 7 -r 15 -o boolean-7.swf boolean.as

#include "values.as"

trace (new Boolean ());
trace (Boolean ());
trace (new Boolean (true));
trace (Boolean (true));
trace (new Boolean (false));
trace (Boolean (false));
trace (new Boolean (true, false));
trace (Boolean (true, false));

for (var i = 0; i < values.length; i++) {
  trace ("Testing: " + names[i]);
  trace (new Boolean (values[i]));
  trace (new Boolean (name[i]));
}

getURL ("FSCommand:quit", "");
