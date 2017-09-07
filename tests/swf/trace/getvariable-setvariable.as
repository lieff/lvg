// makeswf -v 7 -s 200x150 -r 1 -o getvariable-setvariable.swf getvariable-setvariable.as

trace ("Check where setting a variable and then getting it again works");

createEmptyMovieClip ("a", 0);
a.createEmptyMovieClip ("a", 0);
a.a.createEmptyMovieClip ("a", 0);
a.a.a.createEmptyMovieClip ("a", 0);
a.a.a.a.createEmptyMovieClip ("a", 0);

cur = 0;
delimiters = [ ".", ":", "/"];
function check_string (string, rest) {
  for (var i = 0; i < delimiters.length; i++) {
    name = string + delimiters[i] + "a";
    cur++;
    asm {
      push "name"
      getvariable
      push "cur"
      getvariable
      setvariable
      push "x", "name"
      getvariable
      getvariable
      setvariable
    };
    trace (name + " => " + (x == cur ? "YES" : "no"));
    if (rest > 1)
      check_string (name, rest - 1);
  };
};

check_string ("", 5, "YES");
check_string ("a", 4, "YES");

loadMovie ("FSCommand:quit", "");
