<?php
      include_once("../../base_lib.php"); 

$json = file_get_contents('php://input');
$request = json_decode($json, true);
$response = array(
  'keep' => '',
  // 'request' => $request,
);

$conn = BaseLib::dbConn();

$version_id = intval($request['version_id']);
$group_id = intval($request['group_id']);
$parent_id = intval($request['parent_id']);
$response['version_id'] = $version_id;
$response['group_id'] = $group_id;
$response['parent_id'] = $parent_id;
$response['list'] = array();

$stmt = $conn->prepare("
SELECT
  t0.version_id,
  t0.file_group_id,
  t0.id as file_id,
  t0.amount_of_children,
  t1.filepath,
  t1.filename as title
FROM webdiff_files t0
INNER JOIN webdiff_define_files t1 ON t1.id = define_file_id
WHERE t0.version_id = ? AND file_group_id = ? AND t0.parent_id = ?
ORDER BY t0.amount_of_children <> 0 DESC, title
");

$stmt->execute(array($version_id, $group_id, $parent_id));
while ($row = $stmt->fetch()) {
  $response['list'][] = array(
    'version_id' => intval($row['version_id']),
    'group_id' => intval($row['file_group_id']),
    'def_file_id' => intval($row['file_group_id']),
    'amount_of_children' => intval($row['amount_of_children']),
    'file_id' => intval($row['file_id']),
    'title' => $row['title'],
  );
}

header('Content-Type: application/json; charset=utf-8');
echo json_encode($response);
