<?php
      include_once("../../base_lib.php"); 

$json = file_get_contents('php://input');
$request = json_decode($json, true);
$response = array(
  'keep' => '',
  // 'request' => $request,
);

$conn = BaseLib::dbConn();

$version_id = $request['version_id'];
$response['version_id'] = $version_id;
$response['list'] = array();

$stmt = $conn->prepare("
    SELECT
      version_id,
      file_group_id,
      `name` as title,
      COUNT(*) as amount_of_files
    FROM webdiff_files
    INNER JOIN webdiff_file_groups t1 ON t1.id = file_group_id
    WHERE version_id = ?
    GROUP BY version_id, file_group_id
");

$stmt->execute(array($version_id));
while ($row = $stmt->fetch()) {
  $response['list'][] = array(
    'version_id' => intval($row['version_id']),
    'group_id' => intval($row['file_group_id']),
    'title' => $row['title'],
    'amount_of_files' => $row['amount_of_files'],
  );
}

header('Content-Type: application/json; charset=utf-8');
echo json_encode($response);
