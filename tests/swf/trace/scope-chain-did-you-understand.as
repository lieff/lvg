// makeswf -v 7 -s 200x150 -r 1 -o scope-chain-did-you-understand.swf scope-chain-did-you-understand.as
// taken with permission from http://www.timotheegroleau.com/Flash/articles/scope_chain.htm

getMethod = function() {
  var setProto = function() {
    this.__proto__ = o1;
  };
  setProto();

  return function() {
    trace(a);
  };
};

_global.a = 4;
o1 = {a:5};
o2 = {a:6};
a = 7;

o2.theMethod = getMethod();
o2.theMethod();

loadMovie ("FSCommand:quit", "");
