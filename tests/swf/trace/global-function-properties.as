// makeswf -v 7 -r 1 -o global-function-properties-7.swf global-function-properties.as

#include "trace_properties.as"

trace_properties (_global.AsSetupError, "_global", "AsSetupError");
trace_properties (_global.clearInterval, "_global", "clearInterval");
trace_properties (_global.clearTimeout, "_global", "clearTimeout");
trace_properties (_global.enableDebugConsole, "_global", "enableDebugConsole");
trace_properties (_global.escape, "_global", "escape");
trace_properties (_global.isFinite, "_global", "isFinite");
trace_properties (_global.isNaN, "_global", "isNaN");
trace_properties (_global.parseFloat, "_global", "parseFloat");
trace_properties (_global.parseInt, "_global", "parseInt");
trace_properties (_global.setInterval, "_global", "setInterval");
trace_properties (_global.setTimeout, "_global", "setTimeout");
trace_properties (_global.showRedrawRegions, "_global", "showRedrawRegions");
trace_properties (_global.trace, "_global", "trace");
trace_properties (_global.unescape, "_global", "unescape");
trace_properties (_global.updateAfterEvent, "_global", "updateAfterEvent");

loadMovie ("FSCommand:quit", "");
