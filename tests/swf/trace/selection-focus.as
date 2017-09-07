// makeswf -v 7 -s 200x150 -r 1 -o selection-focus.swf selection-focus.as

function check(x) {
  trace (x + " (" + x.focusEnabled + "): " + Selection.setFocus (x));
  trace (Selection.getFocus () + " (" + typeof (Selection.getFocus ()) + ")");
  trace (Selection.setFocus (null));
};
trace (Selection.getFocus () + " (" + typeof (Selection.getFocus ()) + ")");

trace (Selection.setFocus ());
trace (Selection.getFocus ());
check (undefined);

this.focusEnabled = true;
check (this);
createEmptyMovieClip ("a", 0);
check (a);
a.focusEnabled = true;
check (a);
a.focusEnabled = false;
check (a);
a.focusEnabled = true;
check (a);
delete a.tabEnabled;
check (a);
a.__proto__.focusEnabled = true;
check (a);
a.addProperty ("focusEnabled", function () { trace ("getter"); return true; }, null);
check (a);

getURL ("fscommand:quit", "");
