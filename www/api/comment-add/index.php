<?php
      include_once("../../base_lib.php"); 

$json = file_get_contents('php://input');
$request = json_decode($json, true);
$response = array(
  'keep' => '',
  // 'request' => $request,
);

$conn = BaseLib::dbConn();

$define_file_id = $request['define_file_id'];
$comment = $request['comment'];

$stmt = $conn->prepare('
    INSERT INTO webdiff_define_files_comments(define_file_id, comment, dt_created)
    VALUES(?,?,NOW());
');
if ($stmt->execute(array($define_file_id, $comment))) {
  $id =  $conn->lastInsertId();
  $response['id'] = $id;
}
$response['define_file_id'] = $define_file_id;
$response['comment'] = $comment;

header('Content-Type: application/json; charset=utf-8');
echo json_encode($response);
