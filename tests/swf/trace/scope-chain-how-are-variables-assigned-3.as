// makeswf -v 7 -s 200x150 -r 1 -o scope-chain-how-are-variables-assigned-3.swf scope-chain-how-are-variables-assigned-3.as
// taken with permission from http://www.timotheegroleau.com/Flash/articles/scope_chain.htm

o1 = {};
o1.__proto__ = {a:4};
o2 = {};
with (o1) {
  with (o2) {
    trace("The first 'a' reference found is: " + a);
    a = 5;
  };
};
trace(o1.a); // 4
trace(o2.a); // undefined
trace(a); // 5

loadMovie ("FSCommand:quit", "");
