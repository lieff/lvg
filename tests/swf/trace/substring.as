// makeswf -v 7 -s 200x150 -r 1 -o substring.swf substring.as

hi = String ("Hello World");

for (from = -5; from < 20; from++) {
  for (to = -5; to < 20; to++) {
    trace (from + ":" + to + " - " + hi.substring (from, to));
  };
};

loadMovie ("FSCommand:quit", "");
