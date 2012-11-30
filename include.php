<?php


error_reporting(E_ALL);
ini_set('display_errors', '1');


function debug($var) {
  print "<pre>";
  print_r($var);
  print "</pre>";
}


function dbConnect() {

  $host = 'localhost';
  $user = 'root';
  $pass = 'password';
  $db = 'serial';
  
  $con = mysql_connect($host, $user, $pass);
  if (!$con) {
       die('Could not connect: ' . mysql_error());
  }

  mysql_select_db($db, $con);

  return $con;
}


?>