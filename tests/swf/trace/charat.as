// makeswf -v 7 -s 200x150 -r 1 -o charat.swf charat.as

trace ("Check charAt and CharCodeAt");
s = "Grüß Gott Γειά σας שלום 日本語";
for (i = -10; i < 100; i++) {
  trace (s.charAt (i));
  trace (s.charCodeAt (i));
};

loadMovie ("FSCommand:quit", "");
