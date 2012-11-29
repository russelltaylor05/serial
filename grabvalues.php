<?php

include 'include.php';

$filter = $_GET['filter'];

$con = dbConnect();


switch ($filter) {
case 'all':
  $result = mysql_query("SELECT * FROM info ORDER BY time DESC LIMIT 1");
  $row = mysql_fetch_array($result);
  print json_encode($row);
  break;
case 1:
    echo "i equals 1";
    break;
case 2:
    echo "i equals 2";
    break;
}


mysql_close($con);

?>