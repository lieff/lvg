// makeswf -v 7 -r 1 -o propflags-7.swf propflags.as

#include "trace_properties.as"

var o = new_empty_object ();
o[0] = 0;
for (var i = 1; i <= 7; i++) {
  o[i] = i;
  ASSetPropFlags (o, i, i, 0);
}
for (var i = 3; i <= 13; i++) {
  if (i == 11) continue; // flash player bug
  o[1 << i] = 1 << i;
  ASSetPropFlags (o, 1 << i, 1 << i, 0);
}

trace_properties (o, "local", "o");

loadMovie ("FSCommand:quit", "");
