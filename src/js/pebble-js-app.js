Pebble.addEventListener("ready",
	function(e) {
		console.log("JavaScript app ready and running!");
		//Pebble.showSimpleNotificationOnPebble("JavaScript Event Listener", "is ready");
	}
);

// Send command to XBMC JSON-RPC API
function doPOST(data)//, on_load_function)
{
	var req = new XMLHttpRequest();
	
	// TODO: use webstorage to store configuration such as server name
	// (http://www.w3.org/TR/webstorage/).
	
	req.open('POST','http://192.168.178.106:80/jsonrpc');
	req.setRequestHeader("Content-Type", "application/json");
	//if (on_load_function) { req.onload = on_load_function; }
	req.send(data);
}

function doRealSelect()
{
	//Pebble.showSimpleNotificationOnPebble("appmessage receieved", "Select");
	var rpc = {"jsonrpc": "2.0", "method": "Input.Select", "id": "1"};
	return doPOST(JSON.stringify(rpc));
}

function doPlayPause()
{
	// TODO handle more than one active player?
	//Pebble.showSimpleNotificationOnPebble("appmessage receieved", "PlayPause");
	var rpc = {"jsonrpc": "2.0", "method": "Player.PlayPause", "params": { "playerid": "1" }, "id": "1"};
	return doPOST(JSON.stringify(rpc));
}

function doSelect()
{
	var rpc = {"jsonrpc": "2.0", "method": "Player.GetActivePlayers", "id": 1};
	var req = new XMLHttpRequest();
	
	req.open('POST','http://192.168.178.106:80/jsonrpc');
	req.setRequestHeader("Content-Type", "application/json");
	req.onload = function(e) {
		if (req.readyState == 4 && req.status == 200) {
			if(req.status == 200) {
				var response = JSON.parse(req.responseText);
				if (response.result.length > 0) {
					// one (or more) active player(s)
					return doPlayPause();
				}
				// zero active players
				return doRealSelect();
			}
		}
	};
	
	var data = JSON.stringify(rpc);
	req.send(data);
}

function doUp()
{
	//Pebble.showSimpleNotificationOnPebble("appmessage receieved", "Up");
	var rpc = {"jsonrpc": "2.0", "method": "Input.Up", "id": "1"};
	return doPOST(JSON.stringify(rpc));
}

function doDown()
{
	//Pebble.showSimpleNotificationOnPebble("appmessage receieved", "Down");
	var rpc = {"jsonrpc": "2.0", "method": "Input.Down", "id": "1"};
	return doPOST(JSON.stringify(rpc));
}

function doContext()
{
	//Pebble.showSimpleNotificationOnPebble("appmessage receieved", "ContextMenu");
	var rpc = {"jsonrpc": "2.0", "method": "Input.ContextMenu", "id": "1"};
	return doPOST(JSON.stringify(rpc));
}

function doLeft()
{
	//Pebble.showSimpleNotificationOnPebble("appmessage receieved", "Left");
	var rpc = {"jsonrpc": "2.0", "method": "Input.Left", "id": "1"};
	return doPOST(JSON.stringify(rpc));
}

function doRight()
{
	//Pebble.showSimpleNotificationOnPebble("appmessage receieved", "Right");
	var rpc = {"jsonrpc": "2.0", "method": "Input.Right", "id": "1"};
	return doPOST(JSON.stringify(rpc));
}

// Set callback for appmessage events
Pebble.addEventListener("appmessage",
	function(e) {
		//Pebble.showSimpleNotificationOnPebble("appmessage receieved", "AppMessage");
		Pebble.sendAppMessage({"OUTPUT_TEXT":JSON.stringify(e)},
							  function(e) { return; },
							  function(e) { return; }
		);
		if (e.payload.down)         { doDown(); }
		else if (e.payload.up)      { doUp(); }
		else if (e.payload.select)  { doSelect(); }
		else if (e.payload.context) { doContext(); }
		else if (e.payload.left)    { doLeft(); }
		else if (e.payload.right)   { doRight(); }
	}
);

//Pebble.addEventListener("showConfiguration",
//	function(e) {
//		Pebble.openURL("http://bennettp123.com");
//	}
//);
//
//Pebble.addEventListener("webviewclosed",
//	function(e) {
//		var configuration = JSON.parse(e.response);
//		console.log("Configuration window returned: ", configuration);
//	}
//);