var Client = (function() {
  var Client = {};
  var uri = "";
  var timestepList = [];

  Client.requestList = function() {
    console.log("requesting list...");

    $.get(uri + "/list", function(text) {
      var namelist = text.split(/\r?\n/);
      timestepList = [];

      namelist.forEach(function(d) {
        if (d.endsWith(".prop")) {
          timestepList.push(parseInt(d.replace( /^\D+/g, '')));
        };
      });

      timestepList.sort();

      $("#timestepSelector").attr("max", timestepList.length);
      $("#maxTimestepLabel").html(timestepList[timestepList.length - 1]);
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
    }, 3000);
    
    // $('#loading').show();
  }

  Client.requestProp = function(timestep) {
    $.get(uri + "/get?temp_" + ('0000'+timestep).slice(-4) + ".prop", function(text) {
      $("#dataProperties").text(text);
    });
  }

  $("#timestepSelector").change(function() {
    const i = $(this)[0].value;
    const currentTimestep = timestepList[i];
    $("#timestepLabel").html(currentTimestep);

    Client.requestProp(currentTimestep);
  });

  return Client;
})();
