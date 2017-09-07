// makeswf -v 7 -r 1 -o try-finally-7.swf try-finally.as

try {
  try {
    trace ("Try");
  } finally {
    trace ("Finally");
  };
  try {
    throw "Error";
  } finally {
    trace ("Finally");
  };
} catch (e) {
  trace (e);
};

getURL ("FSCommand:quit", "");
