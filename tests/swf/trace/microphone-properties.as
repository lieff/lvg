// makeswf -v 7 -r 1 -o microphone-properties-7.swf microphone-properties.as

#include "trace_properties.as"

var a = new Microphone ();
// FIXME: test with a real microphone instance from Microphone.get

trace_properties (_global.Microphone, "_global", "Microphone");
trace_properties (a, "local", "a");

loadMovie ("FSCommand:quit", "");
