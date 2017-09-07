// makeswf -v 7 -s 200x150 -r 1 -o Transform-matrix.swf Transform-matrix.as

function check (mc) {
  trace ("checking " + mc);
  trace (mc._x);
  trace (mc._y);
  trace (mc._xscale);
  trace (mc._yscale);
  trace (mc._rotation);
  var t = new flash.geom.Transform (mc);
  trace (t.matrix);
};

createEmptyMovieClip ("a", 0);
check (a);

t = new flash.geom.Transform (a);

t.matrix = {};
check (a);

t.matrix = { tx: 10, ty: 20 };
check (a);

t.matrix = { a: 0, b: 1, c: 1, d: 0, tx: 10, ty: 20 };
check (a);

t.matrix = undefined;
check (a);

t.matrix = { a: 1, b: 1, c: 1, d: 0 };
check (a);

t.matrix = { a: 1, b: 3, c: 5, d: 7, tx: 11, ty: 13 };
check (a);

getURL ("fscommand:quit", "");
