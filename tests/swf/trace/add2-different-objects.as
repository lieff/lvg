// makeswf -v 7 -s 200x150 -r 1 -o add2-different-objects.swf add2-different-objects.as

classes = [ "Date", "Number", "String", "Object", "XML", "Boolean" ];

function test (x)
{
  x.valueOf = function () { trace ("valueOf: " + arguments); return x; };
  x.toString = function () { trace ("toString:" + arguments); return x; };
  trace ("String: " + x);
  trace ("Number: " + (0 + x));
  trace ("Self: " + (x + x));
  trace ("== \"hi\": " + (x == "hi"));
  trace ("== 1: " + (x == 1));
  trace ("== self: " + (x == x));
};

for (i = 0; i < classes.length; i++) {
  trace (">>> " + classes[i]);
  x = classes[i];
  asm {
    push "x", "x"
    getvariable
    getvariable
    setvariable
  };
  test (new x ());
}

trace (">>> MovieClip");
test (this);

loadMovie ("fscommand:quit", "");
