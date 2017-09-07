// makeswf -v 7 -r 1 -o array-sort-custom-unique-7.swf array-sort-custom-unique.as

function custom (a, b)
{
  trace ("custom (" + a + ", " + b + ")");
  if (a < b) {
    trace (-1);
    return -1;
  } else if (b < a) {
    trace (1);
    return 1;
  } else {
    trace (0);
    return 0;
  }
}

var a = {};
a.toString = function () { trace ("a toString"); return "a"; };
a.valueOf = function () { trace ("a valueOf"); return "a"; };

var b = {};
b.toString = function () { trace ("b toString"); return "b"; };
b.valueOf = function () { trace ("b valueOf"); return "b"; };

var c = {};
c.toString = function () { trace ("c toString"); return "c"; };
c.valueOf = function () { trace ("c valueOf"); return "c"; };

var d = {};
d.toString = function () { trace ("d toString"); return "d"; };
d.valueOf = function () { trace ("d valueOf"); return "d"; };

var x = [a, c, b, d];
trace (x.sort (custom));
var x = [c, a, c, b];
trace (x.sort (custom, Array.UNIQUESORT));
var x = [a, c, b, d];
trace (x.sort (custom, Array.NUMERIC));
var x = [a, c, b, d];
trace (x.sort (custom, Array.UNIQUESORT | Array.NUMERIC));

var x = [a, c, b, d, c];
trace (x.sort (custom));
var x = [a, c, b, d, c];
trace (x.sort (custom, Array.UNIQUESORT));
var x = [a, c, b, d, c];
trace (x.sort (custom, Array.NUMERIC));
var x = [a, c, b, d, c];
trace (x.sort (custom, Array.UNIQUESORT | Array.NUMERIC));

getURL ("FSCommand:quit", "");
