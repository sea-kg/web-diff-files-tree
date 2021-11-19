<?php
      include_once("../base_lib.php"); 

$json = file_get_contents('php://input');
$request = json_decode($json, true);
$response = array(
  'webdiif_ver' => 'v0.0.1',
  // 'request' => $request,
);

$action = 'None';
if ($request != null && $request['action'] != null) {
  $action = $request['action'];
}
$response['action'] = $action;

$conn = BaseLib::dbConn();


header('Content-Type: application/json; charset=utf-8');
echo json_encode($response);

