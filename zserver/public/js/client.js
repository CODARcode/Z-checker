var Client = (function() {
  var ws;
  var Client = {};
  var uri = "";
  var statsBuffer = [];
  const statsLimit = 100;

  // var plotNames = ["avgAbsErr", "avgPWRErr", "avgRelErr", "avgValue", "compressRate", 
  //   "compressRatio", "compressSize", "compressTime", "decompressRate", "decompressTime", 
  //   "entropy", "maxAbsErr", "maxPWRErr", "maxRelErr", "maxValue", "nrmse", "psnr", "rmse",
  //   "snr"];
  var plotNames = ["psnr", "rmse", "nrmse", "entropy", "avgRelErr", "avgValue", "compressTime", "decompressRate", 
    "ssimImage2D_avg",
    "png-original", "png-decompressed"];

  Client.generatePlots = function() {
    var options = {
      cellHeight: 80,
      verticalMargin: 10
    };
    $('.grid-stack').gridstack(options);

    const blockWidth = 3, blockHeight = 2;
    const blocksPerLine = 4;

    grid = $('.grid-stack').data('gridstack');
    for (i=0; i<plotNames.length; i++) {
      grid.addWidget($('<div><div class="grid-stack-item-content" id="plot-' + plotNames[i] + '" /><div/>'), 
        i%blocksPerLine*blockWidth, i/blocksPerLine*blockHeight, blockWidth, blockHeight);
    }
  }

  Client.generatePlots();

  Client.updateData = function() {
    plotNames.forEach(function(key) {
      if (key.startsWith("png")) return;

      var mydata = [];
      for (i=0; i<statsBuffer.length; i++) 
        mydata.push([statsBuffer[i]["timestep"], statsBuffer[i][key]]);

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
    });
  };

  Client.connectToServer = function(ip, port) {
    uri = 'ws://' + ip + ':' + port;
    ws = new WebSocket(uri);
    Client.ws = ws;
    
    setTimeout(function () {
      if (ws.readyState != 0 && ws.readyState != 1) {
        connectionDialog.error();
        // ws.onerror();
      }
    }, 3000);
    
    // ws.binaryType = "arraybuffer";
    // ws.onopen = onOpen;
    ws.onclose = onClose = function(Evt) {
      connectionDialog.error();
    }
    ws.onerror = function(evt) {
      connectionDialog.error();
    };

    var counter = 0;
    ws.onmessage = function(evt) {
      // console.log(evt);
      var msg = JSON.parse(evt.data);
      if (msg.type == "stats") {
        statsBuffer.push(msg);
        if (statsBuffer.length > statsLimit) 
          statsBuffer.shift();
      } else if (msg.type == "field") {
        $("#plot-png-original").html("<h3>original_data (t=" + msg.timestep + ")</h3><img src='" + msg.original_data + "' />");
        $("#plot-png-decompressed").html("<h3>reconstructed_data (t=" + msg.timestep + ")</h3><img src='" + msg.reconstructed_data + "' />");
        console.log(msg);
      }
      // if (counter ++ % 10 == 0)
      //   Client.updateData(); // control frequency
    }; 

    // Client.requestList();
    setInterval(function() {Client.updateData();}, 1000);
  }

  return Client;
})();
