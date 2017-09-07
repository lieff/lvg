// makeswf -v 7 -r 1 -o style-parsecolor-7.swf style-parsecolor.as

#include "values.as"

tests = [
  "#FF0000",
  "#abcabc",
  "#FFF",
  "#asdefw",
  "#FFXXFF",
  " #FF00FF",
  "#FF00FF ",
  "xFF00FF",
  "white"
];

trace ("Testing parseColor:");

var style = new TextField.StyleSheet ();

trace ("(no param) = " + style.parseColor ());

for (var i = 0; i < values.length; i++) {
  trace (names[i] + " = " + style.parseColor (values[i]));
}

for (var i = 0; i < tests.length; i++) {
  trace (tests[i] + " = " + style.parseColor (tests[i]));
}

// test on non-stylesheet object
var o = new Object ();
o.parseColor = TextField.StyleSheet.prototype.parseColor;
trace (o.parseColor ("#FF00FF"));

loadMovie ("FSCommand:quit", "");
