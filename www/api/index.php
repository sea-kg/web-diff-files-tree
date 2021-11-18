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

if ($action == 'add') {
  $group = $request['group'];
  $group_id = BaseLib::createGroupSafe($group);
  if ($group_id == -1) {
    $response['error'] = 'could not create group safe';
  }
  $version = $request['version'];
  $version_id = BaseLib::createVersionSafe($version);
  
  $files = $request['files'];
  foreach ($files as $filepath) {
    if ($filepath == "") {
      continue;
    }
    $paths = explode("/", $filepath);
    $path = '';
    $parentid = 0;
    foreach($paths as $val) {
      if ($val == '') {
        continue;
      }
      $path .= '/'.$val;
      $parentid = BaseLib::createDefineFileSafe($group_id, $path, $val, $parentid);
      BaseLib::createFileSafe($version_id, $parentid);
    }
  }
}

if ($action == 'loadtree') {
  $load = $request['load'];
  $byid = $request['byid'];
  $response['load'] = $load;
  $response['byid'] = $byid;
  $response['list'] = array();
  if ($load == 'version') {
  }
  if ($load == 'groups') {
    $stmt = $conn->prepare("
      SELECT version_id, deff.file_group_id as child_id, `name` as title, COUNT(*) as count_childs FROM webdiff_files
      INNER JOIN webdiff_define_files deff ON deff.id = define_file_id
      INNER JOIN webdiff_file_groups grs ON grs.id = deff.file_group_id
      WHERE version_id = ?
      GROUP BY version_id, child_id, title
      "
    );
    $stmt->execute(array($byid));
    while ($row = $stmt->fetch()) {
      $response['list'][] = array(
        'uniq_id' => $byid,
        'child_id' => intval($row['child_id']),
        'title' => $row['title'],
        'count_childs' => $row['count_childs'],
      );
    }
  }
  if ($load == 'files') {
    $byid = $request['byid'];
    $version_id = intval(explode(":",$byid)[0]);
    $group_id = intval(explode(":",$byid)[1]);
    $parent_id = intval(explode(":",$byid)[2]);
    $stmt = $conn->prepare("
      SELECT
        t0.id as uniq_id,
        deff.id as child_id,
        `filename` as title,
        deff.childs as count_childs
      FROM `webdiff_files` AS t0
      INNER JOIN
        webdiff_define_files deff ON deff.id = define_file_id
      WHERE
        version_id = ? AND deff.file_group_id = ? AND parent_id = ?
      ORDER BY deff.childs DESC, title
      "
    );
    $stmt->execute(array($version_id, $group_id, $parent_id));
    while ($row = $stmt->fetch()) {
      $response['list'][] = array(
        'uniq_id' => intval($row['uniq_id']),
        'child_id' => intval($row['child_id']),
        'title' => $row['title'],
        'count_childs' => $row['count_childs'],
      );
    }
  }
}

header('Content-Type: application/json; charset=utf-8');
echo json_encode($response);



/*        $stmt = $conn->prepare('SELECT * FROM webdiff_versions ORDER BY `name`');
        $stmt->execute(array());
        while ($row = $stmt->fetch()) {
            $verid = $row['id'];
            $name = $row['name'];
            $comment = $row['comment'];

            echo '<br><div class="card">
              <div class="card-body">
                <h5 class="card-title">#'.$verid.' '.$name.'</h5>
                <h6 class="card-subtitle mb-2 text-muted">'.$comment.'</h6>
              </div>
            </div>';
        }
*/
