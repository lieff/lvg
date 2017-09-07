// makeswf -v 7 -s 200x150 -r 1 -o crash-0.7.1-getTextExtent-refcounting.swf crash-0.7.1-getTextExtent-refcounting.as

tf = new TextFormat ();

for (i = 0; i < 10; i++) {
  tf.getTextExtent ("hi");
}

getURL ("fscommand:quit", "");
