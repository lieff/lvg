// makeswf -v 7 -s 200x150 -r 1 -o function-tostring.swf function-tostring.as

trace ("Check that for functions toString() is called");
function foo () { };
foo.toString = function () {
  return "Hello World!";
};
trace (foo);

loadMovie ("FSCommand:quit", "");
