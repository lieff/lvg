// makeswf -v 7 -s 200x150 -r 1 -o getvariable-dotdot.swf getvariable-dotdot.as

trace ("Check how .. works with 'soft' movieclip references");

a = { toString: function () { return "hi"; } };
a.a = createEmptyMovieClip ("foo", 1);
a.a[".."] = "boom baby";
asm {
  push "a/a/..:a"
  getvariable
  trace
};

loadMovie ("FSCommand:quit", "");
