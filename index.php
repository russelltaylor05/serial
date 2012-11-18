<?php
include 'include.php';

error_reporting(E_ALL);
ini_set('display_errors', '1');

$host = 'localhost';
$user = 'root';
$pass = 'password';
$db = 'serial';

$con = mysql_connect($host, $user, $pass);
if (!$con) {
     die('Could not connect: ' . mysql_error());
}

mysql_select_db($db, $con);

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
    <div>
      <dl id="stats">
        <dt>Temp</dt>
        <dd><?php print $row['temp']; ?></dd>
        <dt>Humidity</dt>
        <dd><?php print $row['humidity']; ?></dd>
        <dt>Pressure</dt>
        <dd><?php print $row['pressure']; ?></dd>
        <dt>light</dt>
        <dd><?php print $row['light']; ?></dd>
      </dl>
    </div>
  </div>  
  
  <!-- JS Scripts -->
  <script src="//ajax.googleapis.com/ajax/libs/jquery/1.8.2/jquery.min.js"></script>
  <script>window.jQuery || document.write('<script src="js/vendor/jquery-1.8.2.min.js"><\/script>')</script>
  <script src="js/main.js"></script>
</body>
</html>


