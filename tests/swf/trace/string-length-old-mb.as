// vivi-compile -v 7 -r 15 -s 200x150 -o string-length-old-7.swf string-length-old.as

var o = {};
o.toString = function () {
  trace ("o.toString");
  return "Hello, world!";
};

asm {
  push "Hello"
  mb_string_length
  trace
  push "älämölöä"
  mb_string_length
  trace
  push 123
  mb_string_length
  trace
  push null
  mb_string_length
  trace
  push "o"
  get_variable
  mb_string_length
  trace
}

getURL ("FSCommand:quit", "");
