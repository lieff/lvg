// makeswf -v 7 -r 1 -o text-field-get-text-format-7.swf text-field-get-text-format.as

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

function format_to_string (fmt) {
  str = "";
  for (var i = 0; i < properties.length; i++) {
    str += " " + properties[i] + "=" + fmt[properties[i]];
  }
  return str;
}

this.createTextField ("t", 1, 0, 0, 200, 200);

t.html = true;
t.htmlText = "<p align='right'>a<b>b<i>c</i>d<u>e</u>f</b>g</p>\rh<font size='20'>i</font>j\r";

// normal
trace ("all:" + format_to_string (t.getTextFormat ()));
for (var i = 0; i < t.length; i++) {
  trace (i + ":" + format_to_string (t.getTextFormat (i)));
  for (var j = i + 1; j < t.length; j++) {
    trace (i + "-" + j + ":" + format_to_string (t.getTextFormat (i, j)));
  }
}

// special cases
trace ("-1:" + format_to_string (t.getTextFormat (-1)));
trace ("-1, 5:" + format_to_string (t.getTextFormat (-1, 5)));
trace ("-1, 30:" + format_to_string (t.getTextFormat (-1, 30)));
trace ("0, -1:" + format_to_string (t.getTextFormat (0, -1)));
trace ("5, 30:" + format_to_string (t.getTextFormat (5, 30)));
trace ("2, 4, 6:" + format_to_string (t.getTextFormat (2, 4, 6)));
trace ("5, 4:" + format_to_string (t.getTextFormat (5, 4)));
trace ("5, 5:" + format_to_string (t.getTextFormat (5, 5)));
trace ("30, 35:" + format_to_string (t.getTextFormat (30, 35)));

loadMovie ("FSCommand:quit", "");
