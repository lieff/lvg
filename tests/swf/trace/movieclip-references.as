// makeswf -v 7 -s 200x150 -r 1 -o movieclip-references.swf movieclip-references.as

function create (mc, array, depth) {
  if (depth == 0)
    return;
  depth--;
  var o = [];
  o.toString = function () { return "[" + Array.prototype.toString.apply (this) + "]"; };
  array.push (o);
  mc.createEmptyMovieClip ("a", 0);
  o.push (mc.a);
  trace (depth + ": begin");
  create (mc.a, array, depth);
  mc.a._name = "b";
  o.push (mc.b);
  trace (depth + ": middle");
  create (mc.b, array, depth);
  mc.b._name = "a";
  o.push (mc.a);
  trace (depth + ": end");
  create (mc.a, array, depth);
  trace (depth + ": " + array);
};

array = [];
create (this, array, 3);
trace (array);
a.removeMovieClip ();
trace (array);
createEmptyMovieClip ("a", 0);
trace (array);
createEmptyMovieClip ("b", 1);
trace (array);
a.createEmptyMovieClip ("b", 0);
a.b._name = "a";
trace (array);
a.a.createEmptyMovieClip ("a", 0);
trace (array);
a.a._name = "b";
trace (array);


getURL ("fscommand:quit", "");
