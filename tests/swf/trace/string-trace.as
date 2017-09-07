// makeswf -v 7 -r 1 -o string-to-string-7.swf string-to-string.as

var a = new String ("internal");

var b = new String ("internal");
b.toString = function () { return "toString"; };

var c = new String ("internal");
c.valueOf = function () { return "valueOf"; };

var d = new String ("internal");
d.toString = function () { return "toString"; };
d.valueOf = function () { return "valueOf"; };

trace (a);
trace ("" + a);

trace (b);
trace ("" + b);

trace (c);
trace ("" + c);

trace (d);
trace ("" + d);

loadMovie ("FSCommand:quit", "");
