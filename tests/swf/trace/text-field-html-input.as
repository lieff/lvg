// makeswf -v 7 -r 1 -o text-field-html-input-7.swf text-field-html-input.as

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

var texts = [
  "a",
  "a\rb",
  // causes problems in my scritps, when converting from dos to unix newlines
  //"a\r\nb",
  "a\n\rb",
  "a\r\rb",
  "a\n\nb",
  "a<p align='right'>b</p>c",
  "a<p align='right'>ä</p>c",
  "a<!-- b -->c",
  "a<!--->b",
  "a<br><li>b</li>c<p>d</p>e<br>",
  "a\r<br>\nb",
  "a<u>b<b>c</b>d</u>e",
  "a<font size='1' color='#ff0000'>b<font size='2' color='#00ff00'>c<font color='#0000ff'>d<u>e",
  "a<p>b",
  "a<li>b</li>c\r<li>d</li>e\n<li>f</li>g<br><li>h</li>i",
  "a<li>b<li>c</li>d</li>e",
  "a<p>b</p>c\r<p>d</p>e\n<p>f</p>g<br><p>h</p>i",
  "a<p>b<p>c</p>d</p>e",
  "a<li>b<p>c</p>d</li>e",
  "a<p align='right'>b<p align='center'>c",
  "a<p>b<u>c</p>d",
  "a<u>b</p>c",
  "a<p>b<li>c</p>d",
  "a<p>b<textformat indent='10'>c</p>d",
  "a<p>b<font size='10'>c</p>d",
  "a<tab>b\t<tab> c"
];

for (var i = 0; i < texts.length; i++) {
  for (var j = 0; j <= 3; j++) {
    this.createTextField ("t", 1, 0, 0, 200, 200);
    t.html = true;
    t.multiline = j & 1;
    t.condenseWhite = j & 2;
    t.htmlText = texts[i];

    trace (i + ": " + texts[i] + ": multiline: " + t.multiline + " condenseWhite: " + t.condenseWhite);

    trace (t.text);
    trace (t.htmlText);

    for (var k = 0; k < t.length; k++) {
      trace (k + "/" + t.text.charAt (k) + ":" + format_to_string (t.getTextFormat (k)));
    }
  }
}

getURL ("FSCommand:quit", "");
