<?php
include 'include.php';

error_reporting(E_ALL);
ini_set('display_errors', '1');

$con = dbConnect();
$result = mysql_query("SELECT * FROM info ORDER BY time DESC LIMIT 1");
$row = mysql_fetch_array($result);
mysql_close($con);

?>

<!DOCTYPE html>
<head>
  <meta charset="utf-8">
  <meta http-equiv="X-UA-Compatible" content="IE=edge,chrome=1">
  <title>Remote Weather Station</title>
  <meta name="description" content="">
  <meta name="viewport" content="width=device-width">
  <link rel="stylesheet" href="styles.css">  
</head>
<body>

  <div id="header">    
    <div>
      <h1>Remote Weather Station</h1>
    </div>
  </div>

  <div id="container">
    <div id="content" class="clearfix">
      <ul id="alerts">
      
      </ul>
  
      <div id="temp" class="panel">
        <div>
          <h2>Tempurature</h2>
          <h3><?php print $row['temp']; ?></h3>
          <div class="chart">
          
          </div>
        </div>
      </div>

      <div id="humidity" class="panel right">
        <div>
          <h2>Humidity</h2>
          <h3><?php print $row['humidity']; ?></h3>
          <div class="chart">
          
          </div>
        </div>
      </div>

      <div id="pressure" class="panel">
        <div>
          <h2>Pressure</h2>
          <h3><?php print $row['pressure']; ?></h3>
          <div class="chart">
          
          </div>
        </div>
      </div>

      <div id="light" class="panel right">
        <div>
          <h2>Light</h2>
          <h3><?php print $row['light']; ?></h3>
          <div class="chart">
          
          </div>
        </div>
      </div>
      <a href="#" id="update">Update</a>
    </div> <!-- /content -->
  </div><!-- /container -->  
  
  <!-- JS Scripts -->
  <script src="//ajax.googleapis.com/ajax/libs/jquery/1.8.2/jquery.min.js"></script>
  <script>window.jQuery || document.write('<script src="js/vendor/jquery-1.8.2.min.js"><\/script>')</script>
  <script src="js/main.js"></script>
</body>
</html>


