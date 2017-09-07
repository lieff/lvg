// makeswf -v 7 -r 1 -o assetnative-7.swf assetnative.as

#include "trace_properties.as"

var o = new Object ();
var empty = new Object ();
ASSetPropFlags (empty, null, 0, 7);
for (var prop in empty) {
  empty[prop] = "to-be-deleted";
  delete empty[prop];
}

ASSetNative (o, 103, "0a,1b,2c,3d,4e,5f,6g,7h,8i,9j,10k,11l");

trace_properties (o, "local", "o");

loadMovie ("FSCommand:quit", "");
