// makeswf -v 7 -s 200x150 -r 1 -o crash-0.6.2-replaceText.swf crash-0.6.2-replaceText.as

createTextField ("t", 0, 0, 0, 200, 150);

t.replaceText (0, 0, "Hell");
t.replaceText (4, 4, "o World");
trace (t.text);
t.replaceText (0, 5, "Hi");
trace (t.text);

getURL ("fscommand:quit", "");
