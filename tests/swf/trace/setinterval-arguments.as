// makeswf -v 7 -s 200x150 -r 1 -o setinterval-arguments.swf setinterval-arguments.as

trace ("Check arguments to setInterval work");
setInterval (function (a, b, c) { trace (a); trace (b); trace (c); }, 7000, 1, 42, "hi");
