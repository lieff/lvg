// makeswf -v 7 -s 200x150 -r 1 -o scope-chain-with-and-scope-chain.swf scope-chain-with-and-scope-chain.as
// taken with permission from http://www.timotheegroleau.com/Flash/articles/scope_chain.htm

o1 = {a:5};
o2 = {};
with (o1) {
  trace(a); // 5
  o2.aMethod = function() {
    trace(a);
  };
};
o2.aMethod(); // undefined

loadMovie ("FSCommand:quit", "");
