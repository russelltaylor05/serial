function updateSingles(){
  var jsonUrl = "grabvalues.php";
  var response = $.getJSON(  
      jsonUrl,  
      {filter: 'all'},  
      function(json) {
        $("#temp h3").html(json.temp + " &deg;F");
        $("#light h3").html(json.light + " mV");
        $("#pressure h3").html(json.pressure + "kPa");
        $("#humidity h3").html(json.humidity);

        if(json.light < 1500) {
          $("#alerts .day").addClass('active');
        }  else {
          $("#alerts .day").removeClass('active');
        }

        if(json.temp > 125) {
          $("#alerts .fire").addClass('active');
        }  else {
          $("#alerts .fire").removeClass('active');
        }

      }  
  );  
}


function chartTemp(chart){
  var jsonUrl = "grabvalues.php";
  var response = $.getJSON(  
      jsonUrl,  
      {filter: 'temp'},  
      function(json) {
        var data = [];
        for(i = 0; i < json.length; i++) {
          data.push([json[i].time, json[i].temp]); 
        }
        chart.setData([data]);
        chart.setupGrid();
        chart.draw();
      }  
  );  
}

function chartLight(chart){
  var jsonUrl = "grabvalues.php";
  var response = $.getJSON(  
      jsonUrl,  
      {filter: 'light'},  
      function(json) {
        var data = [];
        for(i = 0; i < json.length; i++) {
          data.push([json[i].time, json[i].light]); 
        }
        chart.setData([data]);
        chart.setupGrid();
        chart.draw();
      }  
  );  
}

function chartPressure(chart){
  var jsonUrl = "grabvalues.php";
  var response = $.getJSON(  
      jsonUrl,  
      {filter: 'pressure'},  
      function(json) {
        var data = [];
        for(i = 0; i < json.length; i++) {
          data.push([json[i].time, json[i].pressure]); 
        }
        chart.setData([data]);
        chart.setupGrid();
        chart.draw();
      }  
  );  
}

function updatePage() {
  updateSingles();
}

function tempFormatter(v, axis) {

  return v.toFixed(axis.tickDecimals) + " °F";
}

function lightFormatter(v, axis) {
  return v.toFixed(axis.tickDecimals) + " mV";
}

function pressureFormatter(v, axis) {
  return v.toFixed(axis.tickDecimals) + " kPa";
}

function timeFormatter(v, axis) {
  var date = new Date(v*1000);
  return date.getSeconds().toFixed(axis.tickDecimals);
}

$(document).ready(function(){


  var tempOptions = {
      series: { shadowSize: 0 ,lines: {fill: true}},
      xaxis:  {show: false, tickSize: 25,},
      yaxis:  {show: true, min: 50, max: 150, tickFormatter: tempFormatter}
  };

  var lightOptions = {
      series: { shadowSize: 0 ,lines: {fill: true}},
      xaxis:  {show: false, tickSize: 25,},
      yaxis:  {show: true, min: 300, max: 2700, tickFormatter: lightFormatter}
  };

  var pressureOptions = {
      series: { shadowSize: 0 ,lines: {fill: true}},
      xaxis:  {show: false},
      yaxis:  {show: true, min: 50, max: 150, tickFormatter: pressureFormatter}
  };

  
  var tempChart = $.plot($("#temp .chart"), [0], tempOptions);
  chartTemp(tempChart);
  
  var lightChart = $.plot($("#light .chart"), [0], lightOptions);
  chartLight(lightChart);    

  var pressureChart = $.plot($("#pressure .chart"), [0], pressureOptions);
  chartPressure(pressureChart);    
  
  
  setInterval(function() {
    updateSingles();
    chartTemp(tempChart);
    chartLight(lightChart);    
    chartPressure(pressureChart);
  }, 2000);




                                               
});