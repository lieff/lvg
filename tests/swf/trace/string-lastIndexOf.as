// makeswf -v 7 -s 200x150 -r 1 -o string-lastIndexOf.swf string-lastIndexOf.as

trace ("test String.lastIndexOf");
s = "Hello World";

for (j = 0; j < 11; j++) {
  trace ("s.lastIndexOf (\""+ s.substr (j, 1) + "\") = " + s.lastIndexOf (s.substr (j, 1)));
  for (i = -15; i < 15; i++) {
    trace ("s.lastIndexOf (\""+ s.substr (j, 1) + "\", " + i + ") = " + s.lastIndexOf (s.substr (j, 1), i));
  }
};

loadMovie ("FSCommand:quit", "");
