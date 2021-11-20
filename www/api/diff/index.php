<?php
      include_once("../../base_lib.php"); 

$json = file_get_contents('php://input');
$request = json_decode($json, true);
$response = array(
  'keep' => '',
  'groups' => array(),
  // 'request' => $request,
);

$conn = BaseLib::dbConn();

$left_version_id = intval($request['left_version_id']);
$right_version_id = intval($request['right_version_id']);

function load_comments($conn, $define_file_id) {
  $ret = array();

  $stmt = $conn->prepare('
    SELECT
      t0.id,
      t0.define_file_id,
      t0.comment
    FROM webdiff_define_files_comments t0
    WHERE t0.define_file_id = ? AND hided = 0
    ORDER BY dt_created DESC
  ;
  ');
  $stmt->execute(array($define_file_id));
  while ($row = $stmt->fetch()) {
    $comment_id = $row['id'];
    $define_file_id = $row['define_file_id'];
    $comment = $row['comment'];
    $ret[] = array(
      'id' => $comment_id,
      'define_file_id' => $define_file_id,
      'comment' => $comment,
    );
  }
  return $ret;
}

function fill_parent_files($conn, $files, $file_id) {
  if ($file_id == 0) {
    return $files;
  }

  $stmt = $conn->prepare('
  SELECT
    t0.id,
    t0.parent_id,
    t0.file_group_id,
    t0.amount_of_children,
    t0.define_file_id,
    t1.filepath,
    t1.filename
  FROM webdiff_files t0
  INNER JOIN webdiff_define_files t1 ON t1.id = t0.define_file_id
  WHERE t0.id = ?;
  ');
  $stmt->execute(array($file_id));
  if ($row = $stmt->fetch()) {
    $file_id = $row['id'];
    $file_parent_id = $row['parent_id'];
    $file_group_id = $row['file_group_id'];
    $group_name = $row['group_name'];
    $define_file_id = $row['define_file_id'];
    $filepath = $row['filepath'];
    $filename = $row['filename'];
    $amount_of_children = $row['amount_of_children'];
    $group_id = 'group'.$file_group_id;

    $files[] = array(
      'id' => 'id'.$file_id,
      'filename' => $filename,
      'parent' => 'id'.$file_parent_id,
      'group_name' => $group_name,
      'define_file_id' => intval($define_file_id),
      'filepath' => $filepath,
      'amount_of_children' => $amount_of_children,
      'state' => ''
    );
    $files = fill_parent_files($conn, $files, $file_parent_id);
  }
  return $files;
}


function get_diff_files($conn, $left_version_id, $right_version_id, $state, $response) {
  $groups = $response['groups'];
  $stmt = $conn->prepare('
  SELECT
    t0.id,
    t0.parent_id,
    t0.file_group_id,
    t0.amount_of_children,
    t2.name as group_name,
    t0.define_file_id,
    t1.filepath,
    t1.filename
  FROM webdiff_files t0
  INNER JOIN webdiff_define_files t1 ON t1.id = t0.define_file_id
  INNER JOIN webdiff_file_groups t2 ON t2.id = t0.file_group_id
  WHERE version_id = ? AND define_file_id NOT IN (SELECT define_file_id FROM webdiff_files t10 WHERE version_id = ? AND t0.file_group_id = t10.file_group_id);
  ');
  $stmt->execute(array($left_version_id, $right_version_id));
  while ($row = $stmt->fetch()) {
    $file_id = $row['id'];
    $file_parent_id = $row['parent_id'];
    $file_group_id = $row['file_group_id'];
    $group_name = $row['group_name'];
    $define_file_id = $row['define_file_id'];
    $filepath = $row['filepath'];
    $filename = $row['filename'];
    $amount_of_children = $row['amount_of_children'];
    $group_id = 'group'.$file_group_id;
    if (!isset($groups[$group_id])) {
      $groups[$group_id] = array(
        'new' => 0,
        'missing' => 0,
        'title' => $group_name,
        'files' => array(),
      );
    }
    $groups[$group_id][$state] = $groups[$group_id][$state] + 1;
    $groups[$group_id]['files']['id'.$file_id] = array(
      'id' => 'id'.$file_id,
      'filename' => $filename,
      'parent' => 'id'.$file_parent_id,
      'group_name' => $group_name,
      'define_file_id' => intval($define_file_id),
      'filepath' => $filepath,
      'amount_of_children' => $amount_of_children,
      'state' => $state,
      'comments' => load_comments($conn, intval($define_file_id)),
    );
    $files = array();
    $files = fill_parent_files($conn, $files, $file_parent_id);
    // $groups[$group_id]['some'] = $files;
    foreach ($files as $val) {
      $fid = $val['id'];
      if (!isset($groups[$group_id]['files'][$fid])) {
        $groups[$group_id]['files'][$fid] = $val;
      }
    }
  }
  $response['groups'] = $groups;
  return $response;
}

$response = get_diff_files($conn, $left_version_id, $right_version_id, 'missing', $response);
$response = get_diff_files($conn, $right_version_id, $left_version_id, 'new', $response);

header('Content-Type: application/json; charset=utf-8');
echo json_encode($response);
