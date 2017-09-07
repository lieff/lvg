// makeswf -v 7 -r 1 -o date-constructor-7.swf date-constructor.as

#include "values.as"

trace (Date ());
trace (new Date ());

trace (Date (1983, 11, 9));
trace (new Date (1983, 11, 9));

trace (Date (1983, 11, 9, 22, 43, 54, 89));
trace (new Date (1983, 11, 9, 22, 43, 54, 89));

for (var i = 0; i < values.length; i++) {
  // FIXME: Swfdec bug
  if (isFinite (values[i]) && values[i] > 1 << 32) {
    trace ("Skipping: " + names[i]);
    continue;
  }
  trace ("Testing: " + names[i]);
  var d = new Date (1983, 11, values[i], 22, 43, values[i], 89);
  trace (d);
  trace (d.valueOf ());
}

trace (new Date (1983, 11, Infinity, -Infinity, 43, 54, 89));
trace (new Date (1983, 11, -Infinity, Infinity, 43, 54, 89));
trace (new Date (1983, 11, Infinity, NaN, 43, 54, 89));
trace (new Date (1983, 11, -Infinity, NaN, 43, 54, 89));

getURL ("FSCommand:quit", "");
