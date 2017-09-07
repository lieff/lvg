// makeswf -v 7 -r 1 -o camera-properties-7.swf camera-properties.as

#include "trace_properties.as"

var a = new Camera ();
// FIXME: test with a real camera instance from Camera.get

trace_properties (_global.Camera, "_global", "Camera");
trace_properties (a, "local", "a");

loadMovie ("FSCommand:quit", "");
