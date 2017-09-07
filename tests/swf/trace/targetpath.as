// makeswf -v 7 -s 200x150 -r 1 -o targetpath.swf targetpath.as

#include "values.as"

asm {
  push "_root"
  getvariable
  targetpath
  trace
};
createEmptyMovieClip ("foo", 0);
asm {
  push "foo"
  getvariable
  targetpath
  trace
};
foo.createEmptyMovieClip ("foo", 0);
asm {
  push "foo"
  getvariable
  push "foo"
  getmember
  targetpath
  trace
};

for (var i = 0; i < values.length; i++) {
  var a = values[i];
  trace (names[i] + " = " + targetPath (a));
}

loadMovie ("FSCommand:quit", "");
