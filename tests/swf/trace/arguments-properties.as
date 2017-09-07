// makeswf -v 7 -r 1 -o array-properties-7.swf array-properties.as

#include "trace_properties.as"

function test () {
  trace_properties (arguments, "local", "arguments");
}

test ("a", 2);

getURL ("FSCommand:quit", "");
