// makeswf -v 7 -r 1 -o try-jump-7.swf try-jump.as

trace ("Test how Try action handles jumping out of the blocks");

function test() {
  while (true) {
    try {
      break;
    } catch (exception) {
      trace ("inside: " + exception);
    };
  }
  throw new Error ("test");
}

function test2() {
  var i = 0;
  while (true) {
    if (i++ > 5)
      throw new Error ("test2");
    try {
      throw new Error ("catch this");
    } catch (exception) {
      continue;
    } finally {
      trace ("finally");
    };
  }
}

try {
  test ();
} catch (exception) {
  trace ("outside: " + exception);
};

try {
  test2 ();
} catch (exception) {
  trace ("outside: " + exception);
};

loadMovie ("FSCommand:quit", "");
