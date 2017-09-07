// makeswf -v 7 -s 200x150 -r 1 -o scope-chain-on-duplication.swf scope-chain-on-duplication.as
// taken with permission from http://www.timotheegroleau.com/Flash/articles/scope_chain.htm

addFunc = function(obj) {
  var aVariable = new Object();
  aVariable.txt = "Hello there";

  obj.theFunc = function() {
    return aVariable;
  };
};

o1 = new Object();
o2 = new Object();

addFunc(o1);
addFunc(o2);

trace(o1.theFunc().txt); // Hello there
trace(o2.theFunc().txt); // Hello there

trace(o1.theFunc == o2.theFunc) ; // false
trace(o1.theFunc() == o2.theFunc()) ; // false

loadMovie ("FSCommand:quit", "");
