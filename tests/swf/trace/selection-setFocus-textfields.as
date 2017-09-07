// makeswf -v 7 -s 200x150 -r 1 -o selection-setFocus-textfields.swf selection-setFocus-textfields.as

function create (depth, selectable, type) {
  createTextField ("t" + depth, depth, 0, depth * 25, 100, 20);
  var t = this["t" + depth];
  var tf = new TextFormat ();
  tf.font = "Bitstream Vera Sans";
  tf.align = "right";
  t.setNewTextFormat (tf);
  t.border = true;
  t.text = "Hello World";

  t.selectable = selectable;
  t.type = type;
  if (arguments.length > 3)
    t.focusEnabled = arguments[3];
}

create (0, false, "dynamic");
create (1, true, "dynamic");
create (2, false, "input");
create (3, true, "input");
create (4, true, "input", true);
create (5, true, "input", false);

function check (x) {
  Selection.setFocus (x);
  trace (Selection.getFocus () + " (" + Selection.getBeginIndex () + " - " + 
    Selection.getEndIndex () + ") @ " + Selection.getCaretIndex ());
}
check (t0);
check (t1);
check (t2);
check (t3);
check (t4);
check (t5);

createEmptyMovieClip ("a", 100);
check (a);
a.onPress = function () { trace ("press"); };
check (a);

check (undefined);

getURL ("fscommand:quit", "");
