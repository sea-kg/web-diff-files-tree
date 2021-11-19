<?php
      include_once("../../base_lib.php"); 

$json = file_get_contents('php://input');
$request = json_decode($json, true);
$response = array(
  'keep' => '',
  // 'request' => $request,
);

$conn = BaseLib::dbConn();

$group = $request['group'];
$group_id = BaseLib::createGroupSafe($group);
if ($group_id == -1) {
  $response['error'] = 'could not create group safe';
  header('Content-Type: application/json; charset=utf-8');
  echo json_encode($response);
}
$version = $request['version'];
$version_id = BaseLib::createVersionSafe($version);

$files_cache = array();
$files = $request['files'];
foreach ($files as $filefullpath) {
  if ($filefullpath == "") {
    continue;
  }

  $arr_fullpath = explode("/", $filefullpath);
  $def_file_id = 0;
  $file_parent_id = 0;
  $filepath = '';
  foreach($arr_fullpath as $filename) {
    if ($filename == '') {
      continue;
    }
    $filepath .= '/'.$filename;
    
    if (array_key_exists($filepath, $files_cache)) {
      $def_file_id = $files_cache[$filepath];
    } else {
      $def_file_id = BaseLib::createDefineFileSafe($filepath, $filename, $def_file_id);
      $files_cache[$filepath] = $def_file_id;
    }
    $file_parent_id = BaseLib::createFileSafe($version_id, $group_id, $def_file_id, $file_parent_id);
  }


  // TODO double tree define and in files
  // $parentid = BaseLib::createDefineFileSafe($filepath);
  // BaseLib::createFileSafe($version_id, $group_id, $parentid);
}

header('Content-Type: application/json; charset=utf-8');
echo json_encode($response);
