// makeswf -v 7 -r 1 -o style-parsecssfontfamily-7.swf style-parsecssfontfamily.as

#include "values.as"

tests = [
  "mono",
  "sans-serif",
  "serif",
  "mONo",
  "SANS-SERIF",
  "serif "
];

trace ("Testing parseCSSFontFamily:");

var style = new TextField.StyleSheet ();

for (var i = 0; i < values.length; i++) {
  trace (names[i] + " = " + style.parseCSSFontFamily (values[i]));
}

for (var i = 0; i < tests.length; i++) {
  trace (tests[i] + " = " + style.parseCSSFontFamily (tests[i]));
}

// test on non-stylesheet object
var o = new Object ();
o.parseCSSFontFamily = TextField.StyleSheet.prototype.parseCSSFontFamily;
trace (o.parseCSSFontFamily ("mono"));

loadMovie ("FSCommand:quit", "");
