// makeswf -v 7 -s 200x150 -r 1 -o BlurFilter-construct.swf BlurFilter-construct.as

#include "values.as"

trace_filter = function (filter, name) {
  trace (name + ": " + filter.blurX + " / " + filter.blurY + " / " + filter.quality);
};

check = function (val, name) {
  if (arguments.length < 2)
    name = val;
  var filter = new flash.filters.BlurFilter (val, val, val);
  trace_filter (filter, name);
};

trace_filter (new flash.filters.BlurFilter (), "0 args");
trace_filter (new flash.filters.BlurFilter (1), "1 arg");
trace_filter (new flash.filters.BlurFilter (1, 2), "2 args");
trace_filter (new flash.filters.BlurFilter (1, 2, 3), "3 args");
trace_filter (flash.filters.BlurFilter (1, 2, 3), "no new");

for (i = 0; i < values.length; i++) {
  check (values[i], names[i]);
};

check (-0.2);
check (255.125);
check (15.125);

trace (flash.filters.BlurFilter (1, 2, 3));
filter = new flash.filters.BlurFilter (1, 2, 3);
trace_filter (filter);
flash.filters.BlurFilter.apply (filter);
trace_filter (filter);

getURL ("fscommand:quit", "");
