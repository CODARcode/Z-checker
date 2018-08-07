var Client = (function() {
  var Client = {};
  var uri = "";

  var timesteps = [];

  Client.requestList = function() {
    console.log("requesting list...");

    $.get(uri + "/realtime", function(text) {
      data = $.parseJSON(text);
      Client.updatePlot("psnr", data);
    });
  };

  Client.connectToServer = function(ip, port) {
    uri = 'http://' + ip + ':' + port;

    Client.requestList();
    setInterval(function() {Client.requestList();}, 2000);
  }

  Client.updatePlot = function(key, data) {
    var mydata = [];
    for (i=0; i<data["timestep"].length; i++) 
      mydata.push([data["timestep"][i], data[key][i]]);

    var json = {
      title: {text: key},
      xAxis: {
      },
      yAxis: {
        title: { text: key }
      },
      series: [{
        name: key,
        data: mydata,
        animation: false,
        showInLegend: false
      }],
      credits: {enabled: false}
    }

    $("#plot-" + key).highcharts(json);
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
