// makeswf -v 7 -r 15 -o crash-0.6.6-prototype-recursion.swf crash-0.6.6-prototype-recursion.as

trace ("Crash in certain case when prototype recursion limit is exceeded");

function get () { return "hello"; };
function set () { };

var a = {};
a.__proto__.__proto__ = {};
a.__proto__.__proto__.addProperty ("test", get, set);
a.test = "hello2";
