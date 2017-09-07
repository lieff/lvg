// makeswf -v 7 -s 200x150 -r 1 -o getvariable-special.swf getvariable-special.as

trace ("Test some special GetVariable cases");

createEmptyMovieClip ("a", 0);
a.createEmptyMovieClip ("b.c", 0);
a.createEmptyMovieClip ("b..c", 1);
a.createEmptyMovieClip ("..c", 2);
a.createEmptyMovieClip ("..", 3);
a.createEmptyMovieClip ("...", 4);
a.createEmptyMovieClip ("..:", 5);
x = a.createEmptyMovieClip (":", 6);
d = 40;
a.d = 41;
a["b.c"].d = 42;
a["b..c"].d = 43;
a["..c"].d = 44;
a[".."].d = 45;
a["..."].d = 46;
a["..:"].d = 47;
a[":"].d = 48;

asm {
  push "a/b.c:d"
  getvariable
  trace
};
asm {
  push "a/b.c.d"
  getvariable
  trace
};
asm {
  push "a/b.c:::d"
  getvariable
  trace
};
asm {
  push "a/b..c:d"
  getvariable
  trace
};
asm {
  push "a/..c:d"
  getvariable
  trace
};
asm {
  push "a/..:d"
  getvariable
  trace
};
asm {
  push "a/...:d"
  getvariable
  trace
};
asm {
  push "a:...:d"
  getvariable
  trace
};
asm {
  push "...:d"
  getvariable
  trace
};
asm {
  push "a:..::d"
  getvariable
  trace
};
asm {
  push "a:../:d"
  getvariable
  trace
};
asm {
  push ":::a:d"
  getvariable
  trace
};
asm {
  push "a/:/:d"
  getvariable
  trace
};

loadMovie ("FSCommand:quit", "");
