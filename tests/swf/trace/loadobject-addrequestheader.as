// makeswf -v 7 -s 200x150 -r 15 -o loadobject-addrequestheader-7.swf loadobject-addrequestheader.as

function test (o) {
  trace (o._customHeaders);
  trace (o.addRequestHeader ("Key", "Value"));
  trace (o._customHeaders);
  o.addRequestHeader (["Key", "Value"]);
  trace (o._customHeaders);
  o.addRequestHeader ("Key1", "Value1", "Key2", "Value2");
  trace (o._customHeaders);
  o.addRequestHeader (["Key1", "Value1", "Key2", "Value2"]);
  trace (o._customHeaders);
  o.addRequestHeader ("Key");
  trace (o._customHeaders);
  o.addRequestHeader (["Key"]);
  trace (o._customHeaders);
  o.addRequestHeader ();
  trace (o._customHeaders);
  o.addRequestHeader ("Key", 1);
  trace (o._customHeaders);
  o.addRequestHeader (1, "Value");
  trace (o._customHeaders);

  o._customHeaders = "Hello";
  o.addRequestHeader ("Key", "Value");
  trace (o._customHeaders);

  o._customHeaders = new Object ();
  o._customHeaders.push = function () {
    trace ("push: " + arguments);
  };
  o.addRequestHeader ("Key", "Value");
}

trace ("LoadVars");
test (new LoadVars ());
trace ("");
trace ("XML");
test (new XML ());

getURL ("fscommand:quit", "");
