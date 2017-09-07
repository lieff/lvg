// makeswf -v 7 -s 200x150 -r 1 -o movie23.swf movie23.as

createTextField ("t", 0, 0, 0, 200, 150);
var tf = new TextFormat ();
tf.font = "Bitstream Vera Sans";
t.setNewTextFormat (tf);

function sanitize (str) {
  var a = str.split ("\r");
  var b = a.join ("\\r").split ("\n");
  var c = a.join ("\\n").split ("\t");
  return c.join ("\\t");
};

var strings = [
  "<tab>a<br>",
  "<tab><br>",
  "<br>",
  " \t\r\n",
  " \ta\r\n",
  " \ta",
  "a\r\n",
  " a ",
  " a",
  "a ",
  "a",
  " ",
  ""
];

for (var i = 0; i < 1 << 3; i++) {
  t.condenseWhite = i & 1;
  t.multiline = i & 2;
  t.html = i & 3;
  trace ("Testing: condenseWhite: " + t.condenseWhite + " multiline: " + t.multiline + " html: " + t.html);

  for (var j = 0; j < strings.length; j++) {
    var text = strings[j] + "<span>" + strings[j] + "</span>" + strings[j];
    trace (sanitize (text));

    t.htmlText = text;
    trace (t.length + ": " + sanitize (t.text));
    var old_html = t.html;
    t.html = true;
    trace (sanitize (t.htmlText));
    t.html = old_html;

    t.text = text;
    trace (t.length + ": " + sanitize (t.text));
    var old_html = t.html;
    t.html = true;
    trace (sanitize (t.htmlText));
    t.html = old_html;
  }
}

getURL ("fscommand:quit", "");
