// vivi-compile -v 7 -r 15 -s 200x150 -o string-length-old-7.swf string-length-old.as

var o = {};
o.toString = function () {
  trace ("o.toString");
  return "Hello, world!";
};

asm {
  push "Hello", "Hello"
  string_equals
  trace
  push "Hello", "Hallo"
  string_equals
  trace
  push "älämölöä", "älämölöä"
  string_equals
  trace
  push undefined, "undefined"
  string_equals
  trace
  push "Hello, world!", "o"
  get_variable
  string_equals
  trace
}

getURL ("FSCommand:quit", "");
