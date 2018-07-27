function createTable(title, id, data) {
  $("#menu").append("<li><a href='#"+id+"'>"+title+"</a></li>");
  $("#report").append("<div id='"+id+"'><h3>"+title+"</h3></div>");
  $("#report #"+id).append("<table class='table table-striped table-bordered table-hover table-condensed'><thead><tr><th>Property</th><th>Value</th></tr></thead><tbody></tbody></table>");
  var table = $("#report #"+id+" tbody");
  for (var key in data) {
    table.append("<tr><td>" + key + "</td><td>" + data[key] + "</td></tr>");
  }
}

function updateFFTAmp(title, id, data) {
  $("#menu").append("<li><a href='#"+id+"'>"+title+"</a></li>");
  $("#report").append("<div id='"+id+"'><h3>"+title+"</h3><div id='"+id+"Chart'></div></div>");
  
  data.forEach(function(d) {
    if (d[1] < 0.0001) d[1] = 0.0001;
  });

  var json = {
    title: {text: "FFT Amplitude"},
    xAxis: {
      tickInterval: 1
    },
    yAxis: {
      title: { text: "amplitude" },
      min: 0.0001, // avoid zero
      type: "logarithmic",
    },
    series: [{
      name: "FFTAmp",
      data: data
    }],
    credits: {enabled: false}
  }

  $("#"+id+"Chart").highcharts(json);
}

function updateRateDistortion(title, id, data) {
  $("#menu").append("<li><a href='#"+id+"'>"+title+"</a></li>");
  $("#report").append("<div id='"+id+"'><h3>"+title+"</h3><div id='"+id+"Chart'></div></div>");

  console.log(data);
  var json = {
    title: {text: "Rate Distortion"},
    xAxis: {
    },
    yAxis: {
    },
    series: [
      {name: "sz", data: data.sz},
      {name: "zfp", data: data.zfp}
    ],
    credits: {enabled: false}
  };

  $("#"+id+"Chart").highcharts(json);
}

function updateAutoCorr(title, id, data) {
  $("#menu").append("<li><a href='#"+id+"'>"+title+"</a></li>");
  $("#report").append("<div id='"+id+"'><h3>"+title+"</h3><div id='"+id+"Chart'></div></div>");
  
  var json = {
    title: {text: "Autocorrelation"},
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
  
  $("#"+id+"Chart").highcharts(json);
}

function updateDistribution(title, id, data) {
  $("#menu").append("<li><a href='#"+id+"'>"+title+"</a></li>");
  $("#report").append("<div id='"+id+"'><h3>"+title+"</h3><div id='"+id+"Chart'></div></div>");
  
  var d0 = [], d1 = [];
  data.forEach(function(d) {
    d0.push(d[0]);
    d1.push(d[1]);
  });

  var json = {
    title: {text: "Distribution"},
    xAxis: { 
    },
    yAxis: {
      title: { text: "Distribution" }
    },
    series: [
      {name: "d0", data: d0}, 
      {name: "d1", data: d1}
    ],
    credits: {enabled: false}
  }
  
  $("#"+id+"Chart").highcharts(json);
}
