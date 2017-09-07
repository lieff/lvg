// makeswf -v 7 -s 200x150 -r 1 -o string-split-empty.swf string-split-empty.as

trace ("Check String.split() on empty string or without parameters");

var s = "";
var a = s.split ("x");
trace (typeof (a));
trace (a.length);
trace (a);
for (i = -1; i < 2; i++) {
  a = s.split ("x", i);
  trace (typeof (a));
  trace (a.length);
  trace (a);
}

var a = s.split ("");
trace (typeof (a));
trace (a.length);
trace (a);

var a = s.split ();
trace (typeof (a));
trace (a.length);
trace (a);

s = "hello";

var a = s.split ("");
trace (typeof (a));
trace (a.length);
trace (a);

var a = s.split ();
trace (typeof (a));
trace (a.length);
trace (a);

getURL ("FSCommand:quit", "");
