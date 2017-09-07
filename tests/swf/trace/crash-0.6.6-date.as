// makeswf -v 7 -s 100x150 -r 15 -o crash-0.6.6-date-7.swf crash-0.6.6-date.as

var d = new Date(3.0935415006117e+23);
trace (d.getUTCMilliseconds());

getURL ("fscommand:quit", "");
