<?php
      include_once("../base_lib.php"); 

$json = file_get_contents('php://input');
$request = json_decode($json, true);
$response = array(
  'webdiif_ver' => 'v0.0.1',
  // 'request' => $request,
);

header('Content-Type: application/json; charset=utf-8');
echo json_encode($response);

