<?php
      include_once("../../base_lib.php"); 

$json = file_get_contents('php://input');
$request = json_decode($json, true);
$response = array(
  'keep' => '',
  // 'request' => $request,
);

$conn = BaseLib::dbConn();

$comment_id = $request['comment_id'];

$stmt = $conn->prepare('
    UPDATE webdiff_define_files_comments SET hided = 1 WHERE id = ?;
');
$stmt->execute(array($comment_id));

header('Content-Type: application/json; charset=utf-8');
echo json_encode($response);
