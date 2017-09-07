// makeswf -v 7 -s 200x150 -r 1 -o crash-0.7.5-textlayout.swf crash-0.7.5-textlayout.as

createTextField ("t", 0, 0, 0, 200, 150);
t.html = true;
t.htmlText = "12345678901234567890\r1234567890<p align='right'>12345</p>";
trace (t.htmlText);

getURL ("fscommand:quit", "");
