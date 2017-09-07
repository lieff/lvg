// makeswf -v 7 -s 200x150 -r 1 -o getvariable-various.swf getvariable-various.as

trace ("Various random testcases for GetVariable");

createEmptyMovieClip ("a", 0);
a.a = 842;
a.createEmptyMovieClip ("a", 0);
a = new Object ();
a.a = 42;
b = a;

tests = [ 
  "_level0",
  "a/_level0",
  "_root",
  "/_root",
  "/b",
  "/",
  "//",
  "/.a",
  "/a/.a",
  "a/.a",
  "//",
  "/../",
  "../",
  "a/..",
  "a/../",
  "/a",
  "a",
  "a/",
  "a.",
  "a:",
  "a/.",
  "a/:",
  ":a",
  "/:a",
  "/a/a",
  "a/a",
  "a/A",
  "a/_level0",
  "a.a",
  "a:a",
  ":a:a",
  "/a:a",
  "/a/:a",
  "/a/.a"
];

for (var i = 0; i < tests.length; i++) {
  name = tests[i];
  asm {
    push "b", "name"
    getvariable
    getvariable
    setvariable
  };
  trace (name + " => " + b);
};

a[".."] = 100;
asm {
  push "a/.."
  getvariable
  trace
};

loadMovie ("FSCommand:quit", "");
