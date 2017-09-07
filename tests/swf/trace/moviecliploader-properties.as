// makeswf -v 7 -s 200x150 -r 1 -o movie23.swf movie23.as

#include "trace_properties.as"

trace_properties (MovieClipLoader, "_global", "MovieClipLoader");
trace_properties (new MovieClipLoader (), "MovieClipLoader", "instance");

loadMovie ("fscommand:QUIT", "");
