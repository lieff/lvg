// makeswf -v 7 -r 1 -o text-field-values-7.swf text-field-values.as

#include "values.as"

// textColor is limited between 0 and 2^24-1
// type can take "input" or "dynamic"
values.push (17000000);
names.push ("(" + (values.length - 1) + ") 17000000 (number)");
values.push (-17000000);
names.push ("(" + (values.length - 1) + ") -17000000 (number)");
values.push ("input");
names.push ("(" + (values.length - 1) + ") input (string)");
values.push (34000000);
names.push ("(" + (values.length - 1) + ") 34000000 (number)");
values.push (new TextField.StyleSheet ());
names.push ("(" + (values.length - 1) + ") " + values[values.length - 1] + " (StyleSheet)");
values.push (-34000000);
names.push ("(" + (values.length - 1) + ") -34000000 (number)");
values.push ("dynamic");
names.push ("(" + (values.length - 1) + ") dynamic (string)");
values.push ("true");
names.push ("(" + (values.length - 1) + ") true (string)");

// Won't test here:
//  length
//  textHeight
//  textWidth
//  bottomScroll
//  hscroll
//  maxhscroll
//  maxscroll
//  scroll

var properties = [
  // text
  [ "text", "hello", "" ],
  [ "html", false, true ],
  [ "htmlText", "hello", "" ],

  // input
  [ "condenseWhite", false, true ],
  [ "maxChars", 7, 0 ],
  [ "multiline", false, true ],
  [ "restrict", "abc", null ],
  [ "selectable", false, true ],
  //"tabEnabled",
  //"tabIndex",
  [ "type", "input", "dynamic" ],
  [ "variable", "test", null ],

  // border & background
  [ "background", false, true ],
  [ "backgroundColor", 0x00CC00, 0 ],
  [ "border", false, true ],
  [ "borderColor", 0x00CC00, 0 ],

  // scrolling
  [ "mouseWheelEnabled", false, true ],

  // display
  [ "autoSize", "center", false ],
  [ "password", false, true ],
  [ "wordWrap", false, true ],

  // format
  //"antiAliasType",
  [ "embedFonts", false, true ],
  //"gridFitType",
  //"sharpness",
  [ "styleSheet", null, new TextField.StyleSheet () ],
  [ "textColor", 0x00CC00, 0 ]//,
  //"thickness",

  // misc
  //"menu",
  //"filters"
];

this.createTextField ("field", 1, 0, 0, 50, 50);

for (var i = 0; i < properties.length; i++) {
  var prop = properties[i][0];
  field[prop] = properties[i][1];
  trace ("Testing: " + prop + " (default: " + field[prop] + ")");
  for (var j = 0; j < values.length; j++) {
    field[prop] = properties[i][1];
    field[prop] = values[j];
    trace (prop + ": " + names[j] + " = " + field[prop] + " (" +
	typeof (field[prop]) + ")");
    field[prop] = properties[i][2];
    field[prop] = values[j];
    trace (prop + ": " + names[j] + " = " + field[prop] + " (" +
	typeof (field[prop]) + ")");
  }
}

getURL ("FSCommand:quit", "");
