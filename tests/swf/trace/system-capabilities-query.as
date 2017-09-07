// makeswf -v 7 -s 200x150 -r 1 -o system-capabilities-query.swf system-capabilities-query.as

#include "trace_properties.as"

trace ("test System.capabilites.Query");

o = {};
o.Query = ASnative (11, 0);
trace (o.Query());
delete o.Query;
trace_properties (o, "local", "o");

loadMovie ("FSCommand:quit", "");
