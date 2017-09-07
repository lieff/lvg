// makeswf -v 7 -s 200x150 -r 1 -o stringlength.swf stringlength.as

s = "";
trace (s.length);
for (i = 0; i < 1000; i += 2) {
  s = chr (i + 1) + s + chr (i);
  trace (s.length);
}

loadMovie ("FSCommand:quit", "");
