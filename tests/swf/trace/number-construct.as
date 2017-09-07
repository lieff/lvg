// makeswf -v 7 -s 200x150 -r 1 -o number-properties-7.swf number-properties.as

#include "trace_properties.as"

trace (new Number ());
trace (new Number (0));
trace (new Number (1));
trace (new Number (2, 3));
trace (new Number (21239));
trace (new Number (NaN));
trace (new Number (Infinity));
trace (new Number (Infinity));
trace (new Number (-Infinity));

trace (Number ());
trace (Number (0));
trace (Number (1));
trace (Number (2, 3));
trace (Number (21239));
trace (Number (NaN));
trace (Number (Infinity));
trace (Number (Infinity));
trace (Number (-Infinity));

getURL ("FSCommand:quit", "");
