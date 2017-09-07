// makeswf -v 7 -r 1 -o text-field-get-text-format-7.swf text-field-get-text-format.as

var properties = [
  "align",
  "blockIndent",
  "bold",
  "bullet"/*,
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
  "url"*/
];

function format_to_string (fmt) {
  str = "";
  for (var i = 0; i < properties.length; i++) {
    str += " " + properties[i] + "=" + fmt[properties[i]];
  }
  return str;
}

function check () {
  for (var i = 0; i < t.length; i++) {
    trace (i + ":" + format_to_string (t.getTextFormat (i)));
  }
}


this.createTextField ("t", 1, 0, 0, 200, 200);

t.html = true;
t.htmlText = "<p align='right'>a<b>b<i>c</i>d<u>e</u>f</b>g</p>\rh<font size='20'>i</font>j\r";

var fmt = new TextFormat ();

trace ("Starting state:");
check ();

trace ("all, font:");
fmt.font = "A";
trace (t.setTextFormat (fmt));
check ();

trace ("1, bullet:");
fmt.bullet = true;
trace (t.setTextFormat (1, fmt));
check ();

trace ("-3, bullet:");
fmt.bullet = true;
trace (t.setTextFormat (-3, fmt));
check ();

trace ("32, bullet:");
fmt.bullet = true;
trace (t.setTextFormat (32, fmt));
check ();

trace ("3, 6, bullet:");
fmt.bullet = true;
trace (t.setTextFormat (3, 6, fmt));
check ();

trace ("4, 5, !bullet:");
fmt.bullet = false;
trace (t.setTextFormat (4, 5, fmt));
check ();

trace ("-1, 2, bullet:");
fmt.bullet = true;
trace (t.setTextFormat (-1, 2, fmt));
check ();

trace ("7, 30, bullet:");
fmt.bullet = true;
trace (t.setTextFormat (7, 30, fmt));
check ();

trace ("20, 30, !bullet:");
fmt.bullet = false;
trace (t.setTextFormat (20, 30, fmt));
check ();

loadMovie ("FSCommand:quit", "");
