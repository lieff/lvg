// makeswf -v 7 -r 1 -o test-7.swf test.as

#include "values.as"

trace (_root._target);
createEmptyMovieClip ("foo", 0);
trace (foo._target);
foo.createEmptyMovieClip ("foo", 0);
trace (foo.foo._target);

for (var i = 0; i < values.length; i++) {
  trace (names[i] + " = " + values[i]._target);
}

loadMovie ("FSCommand:quit", "");
