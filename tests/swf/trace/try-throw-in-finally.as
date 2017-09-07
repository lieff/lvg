// makeswf -v 7 -r 1 -o try-throw-in-finally-7.swf try-throw-in-finally.as

trace ("Test what exception is thrown out of finally");

try {
  try {
    throw "try";
  } catch (exception) {
    throw "catch";
  } finally {
    throw "finally";
  };
} catch (exception) {
  trace (exception);
};

loadMovie ("FSCommand:quit", "");
