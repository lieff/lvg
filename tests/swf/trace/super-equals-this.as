// makeswf -v 7 -s 200x150 -r 1 -o super-equals-this.swf super-equals-this.as

trace ("Check that super does not equal this");

function foo () {
#if __SWF_VERSION__ > 5
  trace (super === this);
#endif
  trace (super == this);
};

x = new foo();

loadMovie ("FSCommand:quit", "");
