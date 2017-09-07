// makeswf -v 7 -r 1 -o external-interface-properties-7.swf external-interface-properties.as

// enable flash structure for version < 8 too for this test
ASSetPropFlags (_global, "flash", 0, 4096);

#include "trace_properties.as"

var a = new flash.external.ExternalInterface ();

trace_properties (_global.flash.external.ExternalInterface,
    "_global.flash.external", "ExternalInterface");
trace_properties (a, "local", "a");

loadMovie ("FSCommand:quit", "");
