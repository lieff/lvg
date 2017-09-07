// makeswf -v 7 -s 200x150 -r 1 -o extends-properties.swf extends-properties.as

#include "trace_properties.as"

Foo = function () {};
asm {
  push "Foo"
  getvariable
  push "LoadVars"
  getvariable
  extends
};
trace_properties (Foo, "_global", "Foo");

getURL ("fscommand:quit", "");
