// makeswf -v 7 -r 1 -o settimeout-7.swf settimeout.as

function test (name) {
  trace ("hello " + name);
}

function clear_timeout (timeout) {
  trace ("clearing timeout");
  clearTimeout (timeout);
}

function quit (name) {
  trace ("quiting...");
  loadMovie ("FSCommand:quit", "");
}

setTimeout (test, 50, "1");
setTimeout (test, 100, "2");
var a = setTimeout (test, 200, "3");
clearTimeout (a);
var b = setTimeout (test, 300, "4");
setTimeout (clear_timeout, 100, b);
setTimeout (quit, 500);
