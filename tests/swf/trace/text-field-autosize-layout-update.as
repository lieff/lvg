// makeswf -v 7 -s 200x150 -r 1 -o text-field-autosize-layout-update.swf text-field-autosize-layout-update.as

var tf = new TextFormat ();
tf.font = "Bitstream Vera Sans";

for (i = 0; i < 4; i++) {
  createTextField ("t", 0, 0, 0, 50, 25);
  t.setNewTextFormat (tf);
  t.border = true;
  t.wordWrap = true;
  t.text = "Hello World";
  t.scroll = 2;
  t.wordWrap = false;
  if (i % 2)
    t.autoSize = true;
  if (i >> 1)
    var tmp = t._x;
  t.autoSize = false;
  t.wordWrap = true;
  trace (t.scroll);
};

getURL ("fscommand:quit", "");

