// makeswf -v 7 -s 200x150 -r 15 -o test-8.swf test-8.as

var x1 = 0;
var x2 = 0;
var x3 = 0;
var x4 = 0;
var y1 = 0;
var y2 = 0;
var y3 = 0;
var y4 = 0;
var a = { x1: 1, x2: 1, x3: 1, y1: 1, y2: 1, y3: 1 };
with (a) {
  var b = { x1: 2, x2: 2, y1: 2, y2: 2 };
  with (b) {
    var c = { x1: 3, y1: 3 };
    with (c) {
      var x1 = 4;
      var x2 = 4;
      var x3 = 4;
      var x4 = 4;
      var y1;
      var y2;
      var y3;
      var y4;
    }
  }
}
trace (x1);
trace (x2);
trace (x3);
trace (x4);
trace (y1);
trace (y2);
trace (y3);
trace (y4);

getURL ("FSCommand:quit", "");
