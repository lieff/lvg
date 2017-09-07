// makeswf -v 7 -s 200x150 -r 1 -o substring.swf substring.as

hi = String ("ABCDEF");

for (from = -10; from < 10; from++) {
  trace (from + " - " + hi.substr (from));
  for (len = -10; len < 10; len++) {
    trace (from + ":" + len + " - " + hi.substr (from, len));
  };
};

loadMovie ("FSCommand:quit", "");
