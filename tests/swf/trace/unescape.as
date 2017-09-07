// makeswf -v 7 -s 200x150 -r 1 -o unescape.swf unescape.as

for (i = 0; i < 129; i++) {
  for (j = 0; j < 129; j++) {
    s = "%" + String.fromCharCode (i, j);
    trace (s);
    trace (unescape (s));
  }
}

loadMovie ("FSCommand:quit", "");
