// makeswf -v 7 -r 1 -o try-return-finally-7.swf try-return-finally.as

function test () {
  try {
    throw "Try";
  } finally {
    return "Finally";
  };
}

try {
  trace (test ());
} catch (e) {
  trace (e);
};

getURL ("FSCommand:quit", "");
