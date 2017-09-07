// makeswf -v 7 -s 200x150 -r 1 -o onresize.swf onresize.as

trace ("Check no onResize happens when setting scaleMode");

o = { };
o.onResize = function () { 
  trace (this);
  trace (this.foo);
  trace (foo);
};
Stage.addListener (o);
foo = 42;

Stage.scaleMode = "noScale";
Stage.scaleMode = "noBorder";
Stage.scaleMode = "noScale";
Stage.width = 20;

loadMovie ("FSCommand:quit", "");

