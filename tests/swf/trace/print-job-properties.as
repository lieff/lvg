// makeswf -v 7 -r 1 -o print-job-properties-7.swf print-job-properties.as

#include "trace_properties.as"

var a = new PrintJob ();

trace_properties (_global.PrintJob, "_global", "PrintJob");
trace_properties (a, "local", "a");

loadMovie ("FSCommand:quit", "");
