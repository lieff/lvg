// makeswf -v 7 -s 200x150 -r 1 -o scope-chain-this-2.swf scope-chain-this-2.as
// taken with permission from http://www.timotheegroleau.com/Flash/articles/scope_chain.htm

a = 4;
test = function() {
  var a = 5;
  var getRef = function() {
    trace(this.a);
  };
  getRef(); // 5
};
test();

loadMovie ("FSCommand:quit", "");
