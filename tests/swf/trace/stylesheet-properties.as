// makeswf -v 7 -r 1 -o stylesheet-properties-7.swf stylesheet-properties.as

#include "trace_properties.as"

var a = new TextField.StyleSheet ();

trace_properties (_global.TextField.StyleSheet, "_global.TextField", "StyleSheet");
trace_properties (a, "local", "a");

loadMovie ("FSCommand:quit", "");
