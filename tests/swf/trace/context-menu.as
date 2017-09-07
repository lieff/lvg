// makeswf -v 7 -r 1 -o context-menu-7.swf context-menu.as

function trace_cm (cm) {
  trace ("onSelect: " + cm.onSelect);
  trace ("customItems: " + cm.customItems);
  var props = [];
  for (var prop in cm.builtInItems) {
    props.push (prop);
  }
  props.sort ();
  var bi = "builtInItems:";
  for (var i = 0; i < props.length; i++) {
    bi += " " + props[i] + ": " + cm.builtInItems[props[i]];
  }
  trace (bi);
}

trace ("Normal");
var cm = new ContextMenu (Math.pow);
trace_cm (cm);

trace ("No parameters");
cm = new ContextMenu ();
trace_cm (cm);

trace ("Weird parameter");
cm = new ContextMenu (123);
trace_cm (cm);

trace ("Hide built in items");
var cm = new ContextMenu (Math.pow);
cm.hideBuiltInItems ();
trace_cm (cm);

trace ("Copy");
var cm = new ContextMenu (Math.pow);
cm_copy = cm.copy ();
trace ("Original:");
trace_cm (cm);
trace ("Copy:");
trace_cm (cm_copy);

trace ("Add custom to original");
cm.customItems.push ("test");
trace ("Original:");
trace_cm (cm);
trace ("Copy:");
trace_cm (cm_copy);

trace ("Change builtIn in original");
cm.builtInItems.loop = false;
trace ("Original:");
trace_cm (cm);
trace ("Copy:");
trace_cm (cm_copy);

loadMovie ("FSCommand:quit", "");
