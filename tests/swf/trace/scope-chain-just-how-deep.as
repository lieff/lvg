// makeswf -v 7 -s 200x150 -r 1 -o scope-chain-just-how-deep.swf scope-chain-just-how-deep.as
// taken with permission from http://www.timotheegroleau.com/Flash/articles/scope_chain.htm

a1 = 5;
addFunc = function(obj) {
  var a2 = 6;
  var func = function(obj) {
    var a3 = 7;
    var func = function(obj) {
      var a4 = 8;
      var func = function(obj) {
	var a5 = 9;
	obj.retrieve = function(refName) {
	  var a6 = 10;
	  trace(eval(refName));
	};
      };
      func(obj);
    };
    func(obj);
  };
  func(obj);
};
o = {};
addFunc(o);
o.retrieve("a6"); // 10
o.retrieve("a5"); // 9
o.retrieve("a4"); // 8
o.retrieve("a3"); // 7
o.retrieve("a2"); // 6
o.retrieve("a1"); // 5

loadMovie ("FSCommand:quit", "");
