// makeswf -v 7 -s 200x150 -r 1 -o scope-chain-scope-and-prototype-chain.swf scope-chain-scope-and-prototype-chain.as
// taken with permission from http://www.timotheegroleau.com/Flash/articles/scope_chain.htm

a = 5;
addFunc = function(obj) {
  var __proto__ = new Object();
  __proto__.a = 6;
  obj.meth = function() {
    trace(a);
  };
};
o = {};
addFunc(o);
o.meth(); // 6

loadMovie ("FSCommand:quit", "");
