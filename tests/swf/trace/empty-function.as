// makeswf -v 7 -s 200x150 -r 1 -o empty-function.swf empty-function.as

trace ("Just define an empty function and call it. Swfdec likes to crash on that one.");

a = function () {};
a();

loadMovie ("FSCommand:quit", "");
