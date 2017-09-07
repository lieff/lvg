// makeswf -v 7 -r 1 -o point-properties-7.swf point-properties.as

#include "trace_properties.as"

var a = new Button ();
// FIXME: test button embedded in the swf

trace_properties (_global.Button, "_global", "Button");
trace_properties (a, "local", "a");

loadMovie ("FSCommand:quit", "");
