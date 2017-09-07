// vivi-compile -v 7 -r 15 -s 200x150 -o string-greater-7.swf string-greater.as

var o = {};
o.toString = function () {
  trace ("o.toString");
  return "Hello, world!";
};

asm {
  push "Hello", "Hello"
  string_greater
  trace
  push "Hello", "Hallo"
  string_greater
  trace
  push "älämölöä", "älämölöä"
  string_greater
  trace
  push undefined, "undefined"
  string_greater
  trace
  push "Hello, world!", "o"
  get_variable
  string_greater
  trace
  push 2, 1
  string_greater
  trace
  push 2, "1"
  string_greater
  trace
  push "9", 8
  string_greater
  trace
}

getURL ("FSCommand:quit", "");
