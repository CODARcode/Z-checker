var Client = (function() {
  var Client = {};
  var uri = "";

  Client.requestList = function() {
    console.log("requesting list...");

    $.get(uri + "/list", function(data) {
      console.log(data);
    });
  };

  Client.connectToServer = function(ip, port) {
    // console.log("connecting to server...");
    uri = 'http://' + ip + ':' + port;

    Client.requestList();

    setInterval(function () {
      Client.requestList();
      // if (ws.readyState != 0 && ws.readyState != 1) {
      //     ws.onerror();
      // }
    }, 2000);
    
    // $('#loading').show();
  }

  return Client;
})();
