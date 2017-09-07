// makeswf -v 7 -s 200x150 -r 15 -o math-function-valueOf-7.swf math-function-valueOf.as

func = {};
func.valueOf = function () {
    trace ("valueOf ();");
    return 4.5;
};

func2 = {};
func2.valueOf = function () {
    trace ("valueOf2 ();");
    return 5.5;
};

func3 = {};
func3.valueOf = function () {
    trace ("valueOf3 ();");
    return 6.5;
};

var functions = String ("abs,min,max,sin,cos,atan2,tan,exp,log,sqrt,round,random,floor,ceil,atan,asin,acos,pow").split (",");

for (var i = 0; i < functions.length; i++) {
  trace ("Testing: " + functions[i]);
  Math[functions[i]] (func, func2, func3);
}

getURL ("fscommand:quit");
