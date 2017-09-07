// makeswf -v 7 -r 1 -o fromcharcode-other-7.swf fromcharcode-other.as

#include "values.as"

trace ("(no params) = " + String.fromCharCode ());

for (var i = 0; i < values.length; i++) {
  trace (names[i] + " = " + String.fromCharCode (values[i]));
}

loadMovie ("FSCommand:quit", "");
