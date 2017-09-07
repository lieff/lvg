// makeswf -v 7 -r 1 -o textformat-7.swf textformat.as

#include "values.as"

var properties = [
  "align",
  "blockIndent",
  "bold",
  "bullet",
  "color",
  "display",
  "font",
  "indent",
  "italic",
  "kerning",
  "leading",
  "leftMargin",
  "letterSpacing",
  "rightMargin",
  "size",
  "tabStops",
  "target",
  "underline",
  "url"
];

var tests = [
  "left",
  "center",
  "LEFT",
  "right",
  "justify",
  "leFT",
  "inline",
  "block",
  "none",
  "http://www.google.com",
  "_typewriter",
  "sans-serif",
  "_self",
  "_blank",
  "_parent",
  "_asdfa",
  52.5,
  1,
  0,
  -1,
  -52.5,
  NaN,
  Infinity,
  -Infinity,
  undefined,
  "",
  "undefined",
  [],
  [4, 8, 12],
  ["moi", "terve"],
  "4812",
  "4,8,12",
  true,
  false,
  null,
  -2147483648,
  -2147483650,
  2147483648,
  2147483650
];

tests.push (new Array ());
tests.push ([]);
tests.push ([1, 2]);
tests.push ([]);
tests.push (null);
tests.push ([]);
tests.push (new Array ("a", "b", "3", "-2", -2));

var fmt = new TextFormat();

for (var i = 0; i < properties.length; i++) {
  var prop = properties[i];
  trace ("Testing: " + prop);
  trace (fmt[prop]);

  for (var j = 0; j < values.length; j++) {
    fmt[prop] = values[j];
    trace (prop + ": " + names[j] + " = " + fmt[prop] + " (" +
	typeof (fmt[prop]) + ")");
  }

  for (var j = 0; j < tests.length; j++) {
    fmt[prop] = tests[j];
    trace (prop + ": " + tests[j] + " (" + typeof (tests[j]) + ") = " +
	fmt[prop] + " (" + typeof (fmt[prop]) + ")");
  }
}

trace ("Testing constructor:");
fmt = new TextFormat ("str", -2, 0xFF00FF, true, true, true,
    "http://www.google.com", "_self", "center", 2, 2, 2);
for (var i = 0; i < properties.length; i++) {
  trace (properties[i] + " = " + fmt[properties[i]]);
}

loadMovie ("FSCommand:quit", "");
