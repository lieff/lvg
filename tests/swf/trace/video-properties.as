// makeswf -v 7 -r 1 -o video-properties-7.swf video-properties.as

#include "trace_properties.as"

var a = new Video ();

trace_properties (_global.Video, "_global", "Video");
trace_properties (a, "local", "a");

loadMovie ("FSCommand:quit", "");

