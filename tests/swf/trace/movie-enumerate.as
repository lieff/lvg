// makeswf -v 7 -s 200x150 -r 1 -o movie-enumerate.swf movie-enumerate.as

createEmptyMovieClip ("a", 0);
a.createEmptyMovieClip ("a", 0);
function foreach (m) {
  var a = [];
  for (var i in m)
    a.push (i);
  a.sort ();
  trace (a);
  ASSetPropFlags (m, null, 16383);
  a = [];
  for (var j in a)
    a.push (j);
  a.sort ();
  trace (a);
};
foreach (this);
foreach (a);

loadMovie ("fscommand:quit", "");
