// makeswf -v 7 -s 200x150 -r 15 -o math-function-7.swf math-function.as

#include "values.as"

// won't test random here of course
var functions = String ("abs,min,max,sin,cos,atan2,tan,exp,log,sqrt,round,floor,ceil,atan,asin,acos,pow").split (",");

for (var i = 0; i < functions.length; i++) {
  trace ("Testing: " + functions[i]);
  trace (Math[functions[i]] ());
  trace (Math[functions[i]] (1.2));
  trace (Math[functions[i]] (1.2, 0.4));
  trace (Math[functions[i]] (1.2, 0.4, 4.3));
  for (var j = 0; j < values.length; j++) {
    // FIXME: Swfdec bug
    if ((functions[i] == "sin" || functions[i] == "cos" ||
	  functions[i] == "tan" || functions[i] == "pow") &&
	isFinite (values[j]) && values[j] > 100) {
      trace ("Skipping: " + functions[i] + " with " + names[j]);
      continue;
    }
    trace ("Testing: " + functions[i] + " with " + names[j]);
    trace (Math[functions[i]] (values[j], 0.4));
    trace (Math[functions[i]] (0.4, values[j]));
  }
}

getURL ("fscommand:quit");
