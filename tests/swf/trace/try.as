// makeswf -v 7 -r 1 -o try-throw-in-finally-7.swf try-throw-in-finally.as

trace ("Test Try action");

function test() {
  trace ("before");
  try {
    trace ("try");
    throw new Error ("try");
    trace ("try after throw");
  } catch (exception) {
    trace ("catch: " + exception);
    throw new Error ("catch");
    trace ("catch after throw");
  } finally {
    trace ("finally");
  };
  trace ("after");
}

try {
  test ();
} catch (exception) {
  trace ("catch from test: " + exception);
};

loadMovie ("FSCommand:quit", "");
