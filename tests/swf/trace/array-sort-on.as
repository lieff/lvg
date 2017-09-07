// makeswf -v 7 -r 1 -o array-sort-on-7.swf array-sort-on.as

var a = new Array ();
a[0] = { a: 2 };
a[1] = { a: 1 };
a[2] = { a: 3 };
a[0].toString = function () { return "a"; };
a[1].toString = function () { return "b"; };
a[2].toString = function () { return "c"; };

trace (a);
trace (a.sortOn ());
trace (a.sortOn ("a"));

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
trace (a.sortOn (["a", "b", "c"]));

loadMovie ("FSCommand:quit", "");
