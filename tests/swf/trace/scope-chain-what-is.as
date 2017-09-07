// makeswf -v 7 -s 200x150 -r 1 -o scope-chain-what-is.swf scope-chain-what-is.as
// taken with permission from http://www.timotheegroleau.com/Flash/articles/scope_chain.htm

_global.a = 4;
a = 5;
obj = new Object();
obj.a = 6;
with(obj) {
  trace(a); // 6
  delete a;
  trace(a); // 5
  delete a;
  trace(a); // 4
};

loadMovie ("FSCommand:quit", "");
