// makeswf -v 7 -s 200x150 -r 1 -o replaceText-newline.swf replaceText-newline.as

createTextField ("t", 0, 0, 0, 100, 150);
var tf = new TextFormat ();
tf.font = "Bitstream Vera Sans";
t.setNewTextFormat (tf);
t.border = true;

function sanitize (str) {
  var a = str.split ("\r");
  return a.join ("\\r");
};

t.html = true;
t.htmlText = "a\rb";
trace (sanitize (t.text) + " (" + t.length + ")");
t.replaceText (1, 1, "\nc");
trace (sanitize (t.text) + " (" + t.length + ")");
t.replaceText (1, 1, "\rd");
trace (sanitize (t.text) + " (" + t.length + ")");

getURL ("fscommand:quit", "");
