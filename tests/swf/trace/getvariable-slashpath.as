// makeswf -v 7 -s 200x150 -r 1 -o getvariable-slashpath.swf getvariable-slashpath.as

trace ("Check how slash paths are handled with Get/SetVariable");

createEmptyMovieClip ("a", 0);
a.createEmptyMovieClip ("a", 0);
asm {
  push "a/a", 42
  setvariable
  push "a/a"
  getvariable
  trace
};
trace (this["a/a"]);


loadMovie ("FSCommand:quit", "");
