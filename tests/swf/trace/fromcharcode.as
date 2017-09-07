// makeswf -v 7 -s 200x150 -r 1 -o fromcharcode.swf fromcharcode.as

for (i = 0; i < 100000; i += 10) {
  trace (i + ": " + String.fromCharCode (i, i+1, i+2, i+3, i+4, i+5, i+6, i+7, i+8, i+9));
}

loadMovie ("FSCommand:quit", "");
