// makeswf -v 7 -s 200x150 -r 1 -o set-overwrite-global.swf set-overwrite-global.as

trace ("Check the global object doesn't get overwritten when defining variables");
trace (String);
trace (this.String);
String = 42;
trace (String);
trace (this.String);
delete String;
trace (String);
trace (this.String);

loadMovie ("FSCommand:quit", "");
