// makeswf -v 7 -s 200x150 -r 1 -o function-apply-crash.swf function-apply-crash.as

a = [];
a[10000] = "moi";
Object.prototype.isPrototypeOf.apply (this, a);

loadMovie ("FSCommand:quit", "");
