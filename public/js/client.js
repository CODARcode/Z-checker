var Client = (function() {
  var Client = {};
  var uri = "";

  var plotNames = ["avgAbsErr", "avgPWRErr", "avgRelErr", "avgValue", "compressRate", 
    "compressRatio", "compressSize", "compressTime", "decompressRate", "decompressTime", 
    "entropy", "maxAbsErr", "maxPWRErr", "maxRelErr", "maxValue", "nrmse", "psnr", "rmse",
    "snr"];

  plotNames.forEach(function(d) {
    $("body").append('<div class="my-container ui-widget-content"><div id="plot-' + d + '" class="plot-container"></div></div>');
  });
  $(".my-container").draggable({stack: ".products", zIndex: 1000});
  $(".my-container").resizable();

  Client.requestList = function() {
    console.log("requesting list...");

    $.get(uri + "/realtime", function(text) {
      data = $.parseJSON(text);

      plotNames.forEach(function(d) {
        Client.updatePlot(d, data);
      });
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

  return Client;
})();
