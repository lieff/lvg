// makeswf -v 7 -r 1 -o string-object-tostring-7.swf string-object-tostring.as

// check whether toString method is called for string objects when they are
// converted to strings in various places

var str = new String ("str");
str.toString = function () {
  trace ("str toString called");
  return "str.toString";
};
str.valueOf = function () {
  trace ("str valueOf called");
  return "str.valueOf";
};

trace (new String (str));
trace (str.concat (str));

var x = new XML (str);
trace (x);
trace (x.createElement (str));

trace ([str]);

loadMovie ("FSCommand:quit", "");
