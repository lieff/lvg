// makeswf -v 7 -s 200x150 -r 1 -o string-slice.swf string-slice.as

//#include "values.as"

trace ("Check String.slice");

var a = new String ("Hello, world!");
trace (a.slice ());
trace (a.slice (3));
trace (a.slice (3, 7));
trace (a.slice (3, 7, 10));
trace (a.slice (-2, 7));
trace (a.slice (3, -7));

for (var i = 0; i < values.length; i++) {
  trace ("Testing: " + names[i]);
  trace (a.slice (3, values[i]));
}

loadMovie("FSCommand:quit", "");
