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
  
case 'temp':
  $stack = array();
  $result = mysql_query("SELECT id,temp,time FROM info ORDER BY time DESC LIMIT 30");  
  while($row = mysql_fetch_array($result)) {
    $row['time'] = strtotime($row['time']);
    $stack[] = $row;
  }  
  print json_encode($stack);  
  break;

case 'light':
  $stack = array();
  $result = mysql_query("SELECT id, light, time FROM info ORDER BY time DESC LIMIT 30");  
  while($row = mysql_fetch_array($result)) {
    $row['time'] = strtotime($row['time']);
    $stack[] = $row;
  }  
  print json_encode($stack);  
  break;


case 'pressure':
  $stack = array();
  $result = mysql_query("SELECT id, pressure, time FROM info ORDER BY time DESC LIMIT 30");  
  while($row = mysql_fetch_array($result)) {
    $row['time'] = strtotime($row['time']);
    $stack[] = $row;
  }  
  print json_encode($stack);  
  break;

}


mysql_close($con);

?>