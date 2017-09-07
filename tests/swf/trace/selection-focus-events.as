// makeswf -v 7 -s 200x150 -r 1 -o movie23.swf movie23.as

rectangle = function (mc, color, x, y, w, h)
{
  mc.beginFill (color);
  mc.moveTo (x, y);
  mc.lineTo (x, y + h);
  mc.lineTo (x + w, y + h);
  mc.lineTo (x + w, y);
  mc.lineTo (x, y);
  mc.endFill ();
};

function show (string)
{
  trace ("(" + Selection.getFocus() + ") " + string);
};

o = { onKeyDown: function () { show ("keyDown: " + Key.getCode ()); },
      onKeyUp: function () { show ("keyUp: " + Key.getCode ()); } };
Key.addListener (o);
o = { onSetFocus: function (from, to) { show (from + " --> " + to); } };
Selection.addListener (o);


createEmptyMovieClip ("a", 0);
rectangle (a, 0xff, 0, 0, 50, 50);
a.focusEnabled = true;
a.tabEnabled = true;
a.onKillFocus = function(next) { show (next + " <== a"); if (!_root.i) { _root.i = 1; Selection.setFocus (b); }; };
a.onSetFocus = function(next) { show (next + " ==> a"); };
Selection.setFocus (a);
createEmptyMovieClip ("b", 1);
rectangle (b, 0xff, 70, 70, 50, 50);
b.focusEnabled = true;
b.tabEnabled = true;
b.onKillFocus = function(next) { show (next + " <=== b"); if (!_root.i) { _root.i = 1; Selection.setFocus (a); }; };
b.onSetFocus = function(next) { show (next + " ===> b"); };
Selection.setFocus (b);

Selection.setFocus (null);
getURL ("fscommand:quit", "");
