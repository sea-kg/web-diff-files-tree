<?php
      include_once("../../base_lib.php"); 

$json = file_get_contents('php://input');
$request = json_decode($json, true);
$response = array(
  'keep' => '',
  // 'request' => $request,
);

$conn = BaseLib::dbConn();

$response['list'] = array();

$stmt = $conn->prepare('SELECT * FROM webdiff_file_groups ORDER BY `name`');
$stmt->execute(array());
while ($row = $stmt->fetch()) {
    $verid = $row['id'];
    $name = $row['name'];
    $response['list'][] = array(
      'group_id' => intval($row['id']),
      'name' => $row['name'],
    );
}

header('Content-Type: application/json; charset=utf-8');
echo json_encode($response);
