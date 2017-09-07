// vivi-compile -v 7 -r 15 -s 150x200 -o equality-old-7.swf equality-old.as

var o = {};
o.valueOf = function () {
  trace ("o.valueOf");
  return 1;
};

asm {
  push 1, 1
  equals
  trace
  push 1, 2
  equals
  trace
  push 1, "o"
  get_variable
  equals
  trace
  push 0, null
  equals
  trace
}

getURL ("FSCommand:quit", "");
