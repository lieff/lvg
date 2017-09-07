// makeswf -v 7 -r 1 -o array-sort-on-7.swf array-sort-on.as

var a = new Array ();
a[0] = { a: 2, b: 1, c: 1 };
a[1] = { a: 1, b: 4, c: 2 };
a[2] = { a: 3, b: 2, c: 3 };
a[3] = { a: 2, b: 2, c: 3 };
a[4] = { a: 3, b: 2, c: 4 };
a[0].toString = function () { return "a"; };
a[1].toString = function () { return "b"; };
a[2].toString = function () { return "c"; };
a[3].toString = function () { return "d"; };
a[4].toString = function () { return "e"; };

trace (a);

trace (a.concat ().sortOn (["a", "b", "c"], [Array.RETURNINDEXEDARRAY, 0, 0]));
trace (a.concat ().sortOn (["a", "b", "c"], [0, Array.RETURNINDEXEDARRAY, 0]));
trace (a.concat ().sortOn (["a", "b", "c"], [0, 0, Array.RETURNINDEXEDARRAY]));
trace (a.concat ().sortOn (["a", "b", "c"], [Array.RETURNINDEXEDARRAY]));
trace (a.concat ().sortOn (["a", "b", "c"], [Array.RETURNINDEXEDARRAY, 0]));
trace (a.concat ().sortOn (["a", "b", "c"], [Array.RETURNINDEXEDARRAY, 0, 0, 0]));

trace (a.concat ().sortOn (["a", "b", "c"], 8));
trace (a.concat ().sortOn (["a", "b", "c"], "8"));
trace (a.concat ().sortOn (["a", "b", "c"], new String ("8")));

var o = {};
o[0] = Array.RETURNINDEXEDARRAY;
o[1] = 0;
o[2] = 0;
o.length = 3;

trace (a.concat ().sortOn (["a", "b", "c"], o));

trace (a.concat ().sortOn (["a", "b", "c"], [Array.DESCENDING, Array.DESCENDING, Array.DESCENDING]));
trace (a.concat ().sortOn (["a", "b", "c"], [Array.DESCENDING, Array.DESCENDING, 0]));
trace (a.concat ().sortOn (["a", "b", "c"], [0, Array.DESCENDING, Array.DESCENDING]));
trace (a.concat ().sortOn (["a", "b", "c"], [Array.DESCENDING, 0, Array.DESCENDING]));
trace (a.concat ().sortOn (["a", "b", "c"], [Array.DESCENDING, 0, 0]));
trace (a.concat ().sortOn (["a", "b", "c"], [0, Array.DESCENDING, 0]));
trace (a.concat ().sortOn (["a", "b", "c"], [0, 0, Array.DESCENDING]));

// make it non-unique
a[5] = { a: 3, b: 2, c: 3 };
a[5].toString = function () { return "f"; };

trace (a.concat ().sortOn (["a", "b", "c"], [Array.UNIQUESORT, 0, 0]));
trace (a.concat ().sortOn (["a", "b", "c"], [0, Array.UNIQUESORT, 0]));
trace (a.concat ().sortOn (["a", "b", "c"], [0, 0, Array.UNIQUESORT]));
trace (a.concat ().sortOn (["a", "b", "c"], [Array.UNIQUESORT]));
trace (a.concat ().sortOn (["a", "b", "c"], [Array.UNIQUESORT, 0]));
trace (a.concat ().sortOn (["a", "b", "c"], [Array.UNIQUESORT, 0, 0, 0]));

loadMovie ("FSCommand:quit", "");
