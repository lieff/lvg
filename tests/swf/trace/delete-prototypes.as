// makeswf -v 7 -s 200x150 -r 1 -o delete-prototypes.swf delete-prototypes.as

trace ("Check that delete doesn't delete from prototypes.");

x = "this";
this.__proto__.x = "prototype";

trace (x);
delete x;
trace (x);
delete x;
trace (x);

loadMovie ("FSCommand:quit", "");
