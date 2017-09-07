// makeswf -v 7 -r 1 -o test-7.swf test.as

var o = new Object ();
o.toString = function () { return "hello"; };
o.valueOf = function () { return "goodbye"; };

trace ("o: " + o);
trace ("toString: " + o.toString ());
trace ("valueOf: " + o.valueOf ());

o.toUpperCase = String.prototype.toUpperCase;
o.toLowerCase = String.prototype.toLowerCase;
o.charAt = String.prototype.charAt;
o.charCodeAt = String.prototype.charCodeAt;
o.concat = String.prototype.concat;
o.indexOf = String.prototype.indexOf;
o.lastIndexOf = String.prototype.lastIndexOf;
o.slice = String.prototype.slice;
o.substring = String.prototype.substring;
o.split = String.prototype.split;
o.substr = String.prototype.substr;

trace (o.toUpperCase ());
trace (o.toLowerCase ());
trace (o.charAt (3));
trace (o.charCodeAt (2));
trace (o.concat (", ", "and thanks for the fish", "!"));
trace (o.indexOf ("e"));
trace (o.lastIndexOf ("e"));
trace (o.slice (1, 3));
trace (o.substring (1, 3));
trace (o.split ("e"));
trace (o.substr (1, 3));

loadMovie ("FSCommand:quit", "");
