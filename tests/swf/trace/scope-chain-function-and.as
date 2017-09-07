// makeswf -v 7 -s 200x150 -r 1 -o scope-chain-function-and.swf scope-chain-function-and.as
// taken with permission from http://www.timotheegroleau.com/Flash/articles/scope_chain.htm

a = 5;
test = function() {
  trace(a); // 5
  delete a;
  trace(a); // undefined
};

obj = new Object();
obj.a = 6;
obj.meth = test;
obj.meth();

loadMovie ("FSCommand:quit", "");
