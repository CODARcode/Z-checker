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

      timestepList.sort((a, b) => a - b);

      $("#timestepSelector").attr("max", timestepList.length);
      $("#maxTimestepLabel").html(timestepList[timestepList.length - 1]);
    
      // auto update
      currentTimestep = timestepList[timestepList.length - 1];
      Client.requestProp(currentTimestep);
      Client.updateAutocorrPlot(currentTimestep);
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

  function timestepToStr(timestep) {return ('0000'+timestep).slice(-4);};

  Client.requestProp = function(timestep) {
    $.get(uri + "/get?temp_" + timestepToStr(timestep) + ".prop", function(text) {
      $("#dataProperties").text(text);
    });
  };

  Client.updateAutocorrPlot = function(timestep) {
    $.get(uri + "/get?sz(1E-3):temp_" + timestepToStr(timestep) + ".autocorr", function(text) {
      var data = [];
      var allLines = text.split(/\r\n|\n/);
      allLines.shift();
      allLines.forEach(function(l) {
        var strs = l.split(" ");
        if (strs.length == 2) {
          data.push([+strs[0], +strs[1]]);
        }
      });

      var json = {
        title: {text: "autocorr"},
        xAxis: { 
        },
        yAxis: {
          title: { text: "amplitude" }
        },
        series: [{
          name: "autocorr",
          data: data
        }],
        credits: {enabled: false}
      }

      $("#autocorr").highcharts(json);
    });
  };

  $("#timestepSelector").change(function() {
    const i = $(this)[0].value;
    const currentTimestep = timestepList[i];
    $("#timestepLabel").html(currentTimestep);

    Client.requestProp(currentTimestep);
    Client.updateAutocorrPlot(currentTimestep);
  });

  return Client;
})();
