// makeswf -v 7 -s 200x150 -r 1 -o movie23.swf movie23.as

#include "values.as"

rectangle = function (mc, color, x, y, w, h)
{
  mc.beginFill (color);
  mc.moveTo (x, y);
  mc.lineTo (x + w, y);
  mc.lineTo (x + w, y + h);
  mc.lineTo (x, y + h);
  mc.lineTo (x, y);
  mc.endFill ();
};

rectangle (this, 0xFFFF00, 0, 0, 200, 150);
createEmptyMovieClip ("a", 0);

for (i = 0; i < values.length; i++) {
  var x = createEmptyMovieClip ("a" + i, i);
  rectangle (x, values[i], (i % 10) * 20, int (i / 10) * 20, 10, 10);
  trace (x._width);
  trace (x._height);
};

trace (i);
mc = createEmptyMovieClip ("a" + i, i);
x = (i % 10) * 20;
y = int (i / 10) * 20;
w = h = 10;
mc.beginFill ();
mc.moveTo (x, y);
mc.lineTo (x + w, y);
mc.lineTo (x + w, y + h);
mc.lineTo (x, y + h);
mc.lineTo (x, y);
mc.endFill ();
trace (mc._width);
trace (mc._height);
i++;

mc = createEmptyMovieClip ("a" + i, i);
x = (i % 10) * 20;
y = int (i / 10) * 20;
w = h = 10;
mc.beginFill (undefined, 128);
mc.moveTo (x, y);
mc.lineTo (x + w, y);
mc.lineTo (x + w, y + h);
mc.lineTo (x, y + h);
mc.lineTo (x, y);
mc.endFill ();
trace (mc._width);
trace (mc._height);
i++;

mc = createEmptyMovieClip ("a" + i, i);
x = (i % 10) * 20;
y = int (i / 10) * 20;
w = h = 10;
mc.beginFill (0xFF);
mc.moveTo (x, y);
mc.lineTo (x + w, y);
mc.lineTo (x + w, y + h);
mc.lineTo (x, y + h);
mc.lineTo (x, y);
mc.endFill ();
trace (mc._width);
trace (mc._height);
i++;

mc = createEmptyMovieClip ("a" + i, i);
x = (i % 10) * 20;
y = int (i / 10) * 20;
w = h = 10;
mc.beginFill (0xFF, undefined);
mc.moveTo (x, y);
mc.lineTo (x + w, y);
mc.lineTo (x + w, y + h);
mc.lineTo (x, y + h);
mc.lineTo (x, y);
mc.endFill ();
trace (mc._width);
trace (mc._height);
i++;

getURL ("fscommand:quit", "");
