.flash bbox=100x150 filename="text-field-tag-variable.swf" background=white version=8 fps=15

.frame 1
  .edittext text1 width=100 height=100 text="Hello1" variable="variable1"
  .put text1 x=0 y=0
  .edittext text2 width=100 height=100 html text="Hello2" variable="variable2"
  .put text2 x=0 y=0
  .action:
    trace (text1.variable);
    trace (_root.variable1);
    trace (text2.variable);
    trace (_root.variable2);
    _root.variable3 = "Variable";
  .end

.frame 2
  .edittext text3 width=100 height=100 text="TextField" variable="variable3"
  .put text3 x=0 y=0
  .action:
    trace (text3.text);
    trace (_root.variable3);
    _root.variable4 = "Variable4";
    text3.variable = "variable4";
    trace (text3.text);
    trace (_root.variable4);
    text3.text = "Changed";
    trace (text3.text);
    text3.variable = "variable5";
    trace (text3.text);
    trace (_root.variable5);
    getURL ('fscommand:quit', '');
    stop ();
  .end

.end
