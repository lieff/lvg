// makeswf -v 7 -s 200x150 -r 1 -o setinterval-this.swf setinterval-this.as

trace ("Check the this object in setInterval callbacks");
function finish () {
  loadMovie ("FSCommand:quit", "");
};
function foo () {
  trace (this);
  if (this) {
    clearInterval (id2);
    setInterval (finish, 100);
  } else {
    clearInterval (id);
  }
};
id = setInterval (foo, 100);
id2 = setInterval (this, "foo", 200);
