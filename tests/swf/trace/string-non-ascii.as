// makeswf -v 7 -r 15 -o string-non-ascii-7.swf string-non-ascii.as

var s = "ölö äägh";

trace (s.toUpperCase ());
trace (s.toLowerCase ());
trace (s.charAt (5));
trace (s.charCodeAt (5));
trace (s.concat ("å ", "åå", " å!"));
trace (s.indexOf ("ä"));
trace (s.lastIndexOf ("ö"));
trace (s.slice (3, 5));
trace (s.substring (3, 5));
trace (s.split ("ä"));
trace (s.substr (3, 5));

getURL ("FSCommand:quit", "");
