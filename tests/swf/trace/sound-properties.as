// makeswf -v 7 -r 1 -o sound-properties-7.swf sound-properties.as

#include "trace_properties.as"

var a = new Sound ();

trace_properties (_global.Sound, "_global", "Sound");
trace_properties (a, "local", "a");

loadMovie ("FSCommand:quit", "");
