// makeswf -v 7 -r 1 -o loadobject-addrequestheader-propflags-7.swf loadobject-addrequestheader-propflags.as

#include "trace_properties.as"

var a = new LoadVars ();
a.addRequestHeader ("Key", "Value");
trace_properties (a, "local", "a");

var a = new XML ();
a.addRequestHeader ("Key", "Value");
trace_properties (a, "local", "a");

getURL ("FSCommand:quit", "");
