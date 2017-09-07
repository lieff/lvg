// vivi-compile -v 7 -s 200x150 -r 15 -o crash-0.7.1-throw.swf crash-0.7.1-throw.as

trace ("Throw as next action after throwing from ending block");

try {
  asm {
    try has_finally "" finally_start finally_start end
    push "Try"
    throw
  finally_start:
    push "Catch2"
  end:
    throw
  }
} catch (e) {
  trace (e);
}

getURL ("fscommand:quit", "");
