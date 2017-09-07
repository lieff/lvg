// makeswf -v 7 -s 200x150 -r 1 -o text-field-hscroll.swf text-field-hscroll.as

createTextField ("t", 0, 0, 0, 100, 32);
var tf = new TextFormat ();
tf.font = "Bitstream Vera Sans";
t.setNewTextFormat (tf);
t.border = true;

t.text = "Hello\nWorld\nHey";
tf.size = 24;
t.setTextFormat (0, 5, tf);

trace (t.scroll);
trace (t.bottomScroll);
trace (t.maxscroll);

createTextField ("s", 1, 0, 50, 100, 10);
var tf = new TextFormat ();
tf.font = "Bitstream Vera Sans";
s.setNewTextFormat (tf);
s.border = true;

s.text = "Hello World";

trace (s.scroll);
trace (s.bottomScroll);
trace (s.maxscroll);

getURL ("fscommand:quit", "");
