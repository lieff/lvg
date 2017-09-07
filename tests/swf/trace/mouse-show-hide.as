// makeswf -v 7 -s 200x150 -r 1 -o mouse-show-hide.swf mouse-show-hide.as

trace (Mouse.hide ());
trace (Mouse.hide ());
trace (Mouse.show ());
trace (Mouse.show ());

loadMovie ("FSCommand:quit", "");
