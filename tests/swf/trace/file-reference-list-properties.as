// makeswf -v 7 -r 1 -o file-reference-list-properties-7.swf file-reference-list-properties.as

#include "trace_properties.as"

// enable flash structure for version < 8 too for this test
ASSetPropFlags (_global, "flash", 0, 5248);

var a = new flash.net.FileReferenceList ();

trace_properties (_global.flash.net.FileReferenceList, "_global.flash.net", "FileReferenceList");
trace_properties (a, "local", "a");

loadMovie ("FSCommand:quit", "");
