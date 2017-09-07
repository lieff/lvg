// makeswf -v 7 -s 200x150 -r 1 -o settarget-delimiters.swf settarget-delimiters.as

trace ("test setTarget action");

createEmptyMovieClip ("a", 0);
a.createEmptyMovieClip ("a", 0);
a.a.createEmptyMovieClip ("a", 0);
a.a.a.createEmptyMovieClip ("a", 0);
a.a.a.a.createEmptyMovieClip ("a", 0);

delimiters = [ ".", ":", "/" ];
names = [ "", "..", "a" ];
function check_string (string, rest) {
  for (var i = 0; i < delimiters.length; i++) {
    for (var j = 0; j < names.length; j++) {
      var name = string + delimiters[i] + names[j];
      setTarget (name);
      trace (name + " => " + _target);
      setTarget ("");
      if (rest > 1)
	check_string (name, rest - 1);
    };
  };
};

check_string ("", 4);

trace ("and now again with feeling!");
a.a.a.a.a = {toString: function () { return "a.a.a.a.a"; } };
a.a.a.a = {toString: function () { return "a.a.a.a"; } };
a.a.a = {toString: function () { return "a.a.a"; } };
a.a = {toString: function () { return "a.a"; } };
a = {toString: function () { return "a"; } };
check_string ("", 4);

loadMovie ("FSCommand:quit", "");
