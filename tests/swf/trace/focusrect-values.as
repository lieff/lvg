// makeswf -v 7 -s 200x150 -r 1 -o focusrect-values.swf focusrect-values.as

#include "values.as"

createEmptyMovieClip ("a", 0);
createEmptyMovieClip ("b", 1);
trace ("initial: " + a._focusrect + " / " + b._focusrect + " / " + _focusrect);
for (i = 0; i < values.length; i++) {
  a._focusrect = values[i];
  trace (names[i] + ": " + a._focusrect + " / " + b._focusrect + " / " + _focusrect);
  _focusrect = false;
  _focusrect = values[i];
  trace (names[i] + "[2]: " + a._focusrect + " / " + b._focusrect + " / " + _focusrect);
  _focusrect = true;
  _focusrect = values[i];
  trace (names[i] + "[3]: " + a._focusrect + " / " + b._focusrect + " / " + _focusrect);
}

getURL ("fscommand:quit", "");
