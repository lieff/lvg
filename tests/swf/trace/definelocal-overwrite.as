// makeswf -v 7 -s 200x150 -r 15 -o test-8.swf test-8.as

var x = 0;
trace (x);
var x;
trace (x);
var x = 1;
trace (x);

getURL ("FSCommand:quit", "");
