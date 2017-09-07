// vivi-compile -v 7 -r 15 -s 200x150 -o string-length-old-7.swf string-length-old.as

var o = {};
o.toString = function () {
  trace ("o.toString");
  return "Hello, world!";
};

asm {
  push "Hello"
  string_length
  trace
  push "älämölöä"
  string_length
  trace
  push 123
  string_length
  trace
  push null
  string_length
  trace
  push "o"
  get_variable
  string_length
  trace
}

getURL ("FSCommand:quit", "");
