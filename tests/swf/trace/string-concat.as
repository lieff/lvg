// makeswf -v 7 -s 200x150 -r 1 -o string-concat.swf string-concat.as

#include "values.as"

trace ("Check String.concat");

var a = new String ("Hello");
trace (a.concat ());
trace (a.concat (", world!"));
trace (a.concat (", ", "world", "!"));

for (var i = 0; i < values.length; i++) {
  trace ("Testing: " + names[i]);
  trace (a.concat (", ", values[i], "!"));
}

loadMovie("FSCommand:quit", "");
