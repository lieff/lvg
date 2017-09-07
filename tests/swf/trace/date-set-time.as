// makeswf -v 7 -s 200x150 -r 15 -o date-set-time-7.swf date-set-time.as

#include "values.as"

trace ("Go");

var d = new Date (2000, 8, 3);
trace (d.setTime ());
trace (d);
trace (d.valueOf ());

trace (d.setTime (123, 12345));
trace (d);
trace (d.valueOf ());

trace (d.setTime (123124.4));
trace (d.valueOf ());

d.setTime (123124.5);
trace (d.valueOf ());

d.setTime (123124.6);
trace (d.valueOf ());

d.setTime (-123124.4);
trace (d.valueOf ());

d.setTime (-123124.5);
trace (d.valueOf ());

d.setTime (-123124.6);
trace (d.valueOf ());

for (var i = 0; i < values.length; i++) {
  trace ("Testing: " + names[i]);
  delete d;
  var d = new Date ();
  d.setTime (values[i]);
  trace (d);
  trace (d.valueOf ());
}

getURL ("fscommand:quit");
