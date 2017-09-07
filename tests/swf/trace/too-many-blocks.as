// makeswf -v 7 -s 200x150 -r 15 -o test-8.swf test-8.as

var x1 = 0;
var x2 = 0;
var x3 = 0;
var x4 = 0;
var a = { x1: 1, x2: 1, x3: 1 };
with (a) {
  var b = { x1: 2, x2: 2 };
  with (b) {
    var c = { x1: 3 };
    with (c) {
      x1 = 4;
      x2 = 4;
      x3 = 4;
      x4 = 4;
    }
  }
}
trace (x1);
trace (x2);
trace (x3);
trace (x4);

getURL ("FSCommand:quit", "");
