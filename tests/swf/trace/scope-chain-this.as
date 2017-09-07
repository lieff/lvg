// makeswf -v 7 -s 200x150 -r 1 -o scope-chain-this.swf scope-chain-this.as
// taken with permission from http://www.timotheegroleau.com/Flash/articles/scope_chain.htm

a = 5;
test = function() {
    trace(this.a);
};

obj = new Object();
obj.a = 6;
obj.meth = test;

obj2 = new Object();
obj2.a = 7;
obj2.meth = test;

obj.meth(); // 6
obj2.meth(); // 7

loadMovie ("FSCommand:quit", "");
