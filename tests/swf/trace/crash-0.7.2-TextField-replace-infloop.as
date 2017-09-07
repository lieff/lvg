// makeswf -v 7 -s 200x150 -r 1 -o crash-0.7.2-TextField-replace-infloop.swf crash-0.7.2-TextField-replace-infloop.as

var tf = new TextFormat ();
tf.font = "Bitstream Vera Sans";

createTextField ("t", 0, 0, 0, 50, 25);
t.setNewTextFormat (tf);
t.wordWrap = true;
t.text = "Hello World";

t.replaceText (6, 11, "a");
t.replaceText (0, 0, "Yo, ");
trace (t.text);

getURL ("fscommand:quit", "");
