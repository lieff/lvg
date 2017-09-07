// makeswf -v 7 -s 200x150 -r 1 -o getvariable-delimiters.swf getvariable-delimiters.as

trace ("test delimiters for GetVariable");

createEmptyMovieClip ("a", 0);
a.createEmptyMovieClip ("a", 0);
a.a.createEmptyMovieClip ("a", 0);
a.a.a.createEmptyMovieClip ("a", 0);
a.a.a.a.createEmptyMovieClip ("a", 0);
b = 1;
a.b = 2;
a.a.b = 3;
a.a.a.b = 4;
a.a.a.a.b = 5;
a.a.a.a.a.b = 6;

delimiters = [ ".", ":", "/"];
function check_string (string, rest, last) {
  for (var i = 0; i < delimiters.length; i++) {
    name = string + delimiters[i] + "b";
    asm {
      push "b", "name"
      getvariable
      getvariable
      setvariable
    };
    b = b ? "YES" : "no";
    name = string + delimiters[i] + "a";
    asm {
      push "x", "name"
      getvariable
      getvariable
      setvariable
    };
    trace (name + " => " + (x ? "YES" : "no") + " (" + x + " / " + b + ") - " + last);
    x = x ? "YES" : "no";
    if (rest > 1)
      check_string (name, rest - 1, x);
  };
};

check_string ("", 5, "YES");
check_string ("a", 4, "YES");

trace ("and now again with feeling!");
a.a.a.a.a = {toString: function () { return "a.a.a.a.a"; } };
a.a.a.a = {toString: function () { return "a.a.a.a"; } };
a.a.a = {toString: function () { return "a.a.a"; } };
a.a = {toString: function () { return "a.a"; } };
a = {toString: function () { return "a"; } };
check_string ("", 5, "YES");
check_string ("a", 4, "YES");

loadMovie ("FSCommand:quit", "");
