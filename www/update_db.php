<?php

$curdir_update_db = dirname(__FILE__);
include_once ($curdir_update_db."/base_lib.php");

$conn = BaseLib::dbConn();

$current_db_ver = 0;

$stmt = $conn->prepare('SELECT * FROM webdiff_db_updates ORDER BY dt DESC LIMIT 0,1');
$stmt->execute(array());

if ($row = $stmt->fetch()) {
  $current_db_ver = $row['version'];
}
$response['current_db_ver'] = $current_db_ver;
$response['apply_updates'] = array();

function next_func($cv) {
  return 'update'.str_pad("".($cv+1), 4, "0", STR_PAD_LEFT);
}

$funcname = next_func($current_db_ver);
$fileupdate = $funcname.'.php';

while (file_exists($curdir_update_db.'/db_updates/'.$fileupdate)) {
  include_once($curdir_update_db.'/db_updates/'.$fileupdate);
  if (function_exists($funcname)) {
    $response['apply_updates'] = $funcname;
    if ($funcname($conn)) {
      $stmt = $conn->prepare('INSERT INTO webdiff_db_updates(version,dt) VALUES(?,NOW())');
      $stmt->execute(array($current_db_ver+1));
      $response['current_db_ver'] = $current_db_ver+1;
    } else {
      StaticLib::error(500, "Failed update");
    }
  }
  // next update
  $current_db_ver++;
  $funcname = next_func($current_db_ver);
  $fileupdate = $funcname.'.php';
}