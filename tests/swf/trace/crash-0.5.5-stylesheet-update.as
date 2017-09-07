// makeswf -v 7 -s 200x150 -r 1 -o crash-0.5.5-stylesheet-update.swf crash-0.5.5-stylesheet-update.as

createTextField ("t", 0, 0, 0, 200, 150);
s = new TextField.StyleSheet ();
t.styleSheet = s;
s.update ();

loadMovie ("fscommand:quit", "");
