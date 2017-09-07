// makeswf -v 7 -s 200x150 -r 1 -o scope-chain-on-activation-object-persistance.swf scope-chain-on-activation-object-persistance.as
// taken with permission from http://www.timotheegroleau.com/Flash/articles/scope_chain.htm

test = function(obj) {
  var aVariable_1 = "Hello";
  var aVariable_2 = "There";
  var aVariable_3 = "Tim";
  obj.retrieve = function(refName) {
    trace(eval(refName));
  };
};
o = new Object();
test(o);
o.retrieve("aVariable_1"); // Hello
o.retrieve("aVariable_2"); // There
o.retrieve("aVariable_3"); // Tim

loadMovie ("FSCommand:quit", "");
