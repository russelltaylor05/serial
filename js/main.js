

function updateSingles(){

  var jsonUrl = "grabvalues.php"; 
  var result;
  var response;  

  var response = $.getJSON(  
      jsonUrl,  
      {filter: 'all'},  
      function(json) {
        $("#temp h3").html(json.temp);
        $("#light h3").html(json.light);
        $("#pressure h3").html(json.pressure);
        $("#humidity h3").html(json.humidity);                
      }  
  );  
}

$(document).ready(function(){


$('#update').click(function(event){
  event.preventDefault();
  updateSingles();
});


                                               
});