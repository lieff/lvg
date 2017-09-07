// makeswf -v 7 -s 200x150 -r 1 -o double-to-string.swf double-to-string.as

trace ("Check various things for double to string conversion");
for (i = 1; i > 0.000000000000001; i /= 2) {
  x = 1 / i + i;
  trace (i);
  trace (-i);
  trace (x);
  trace (-x);
}
for (i = 1; i > 0.000000000000001; i /= 3) {
  trace (i);
  trace (-i);
}
for (i = 1; i > 0.000000000000001; i /= 2) {
  x = 1 + i;
  trace (x);
  trace (-x);
};
for (i = 1; i < 10000000000000000; i *= 2) {
  x = i + 0.0000001;
  trace (i);
  trace (-i);
  trace (x);
  trace (-x);
};
for (i = 1; i < 10000000000000000; i *= 10) {
  trace (i);
  trace (-i);
}

loadMovie ("FSCommand:quit", "");
