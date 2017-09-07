// makeswf -v 7 -r 1 -o array-movieclip-7.swf array-movieclip.as

var a = this.createEmptyMovieClip ("a", 0);
a[0] = "b";
a[1] = "a";
a.length = 2;

ASSetPropFlags (a, null, 0, 7);
ASSetPropFlags (Array.prototype, null, 0, 7);
for (var prop in Array.prototype) {
  a[prop] = Array.prop;
}

ASSetNative (Array.prototype, 252, "push,pop,concat,shift,unshift,slice,join,splice,toString,sort,reverse,sortOn", 1);

var str = "";
for (var i = 0; i < a.length; i++) {
  str += a[i] + ",";
}
trace (str);

trace (a);
trace (a.push ("c"));
trace (a.pop ());
trace (a.concat (["d", "e"]));
trace (a.shift ());
trace (a.unshift ("f"));
trace (a.slice (1, 2));
trace (a.join (":"));
trace (a.splice (1, 1, "g"));
trace (a.toString ());
trace (a.sort ());
trace (a.reverse ());
trace (a.sortOn ("a"));

var str = "";
for (var i = 0; i < a.length; i++) {
  str += a[i] + ",";
}
trace (str);

loadMovie ("FSCommand:quit", "");
