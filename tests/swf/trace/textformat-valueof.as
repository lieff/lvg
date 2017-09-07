// makeswf -v 7 -r 1 -o textformat-7.swf textformat.as

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


var tester_counter = 0;
var tester = new Object ();

tester.valueOf = function () {
  tester_counter++;
  trace ("valueOf: " + tester_counter);
  return tester_counter;
};

tester.toString = function () {
  tester_counter++;
  trace ("toString: " + tester_counter);
  return "toString_" + tester_counter;
};


var fmt = new TextFormat();

for (var i = 0; i < properties.length; i++) {
  var prop = properties[i];
  trace ("Testing: " + prop);
  fmt[prop] = tester;
  trace (fmt[prop]);
}


var align_tester_counter = 0;
var align_tester = new Object ();

align_tester.toString = function () {
  align_tester_counter++;
  trace ("aling_toString: " + align_tester_counter);
  if (align_tester_counter % 4 == 0) {
    return "left";
  } else if (align_tester_counter % 3 == 0) {
    return "center";
  } else if (align_tester_counter % 2 == 0) {
    return "right";
  } else {
    return "justify";
  }
};

trace ("Testing align:");
for (var i = 0; i < 5; i++) {
  fmt["align"] = align_tester;
  trace (fmt["align"]);
}


var display_tester_counter = 0;
var display_tester = new Object ();

display_tester.toString = function () {
  display_tester_counter++;
  trace ("display_toString: " + display_tester_counter);
  if (display_tester_counter % 3 == 0) {
    return "none";
  } else if (display_tester_counter % 2 == 0) {
    return "inline";
  } else {
    return "block";
  }
};

trace ("Testing display:");
for (var i = 0; i < 5; i++) {
  fmt["display"] = display_tester;
  trace (fmt["display"]);
}


loadMovie ("FSCommand:quit", "");
