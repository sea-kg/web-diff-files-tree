<?php
date_default_timezone_set('UTC');
$curdir_gt = dirname(__FILE__);

class BaseLib {
  static $TOKEN = null;
  static $CONN = null;
  static $CONFIG = null;
  static $ROLE = null;
  static $USERID = null;
  static $USERNAME = null;
  static $SETTINGS = null;

  static function dbConn() {
    if (BaseLib::$CONN != null)
      return BaseLib::$CONN;
    
      BaseLib::$CONN = new PDO(
      'mysql:host='.BaseLib::$CONFIG['conn']['host'].';dbname='.BaseLib::$CONFIG['conn']['db'].';charset=utf8',
      BaseLib::$CONFIG['conn']['username'],
      BaseLib::$CONFIG['conn']['password']
    );
    return BaseLib::$CONN;
  }
  
  static function getRandomString($length = 10) {
    mt_srand((double)microtime()*10000);//optional for php 4.2.0 and up.
    $characters = '0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ';
    $charactersLength = strlen($characters);
    $randomString = '';
    for ($i = 0; $i < $length; $i++) {
      $randomString .= $characters[rand(0, $charactersLength - 1)];
    }
    return $randomString;
  }
  
  static function isAuthorized() {
    return BaseLib::$TOKEN != null;
  }
  
  static function startAdminPage() {
    if (isset($_COOKIE['gt_admin_token'])) {
      BaseLib::$TOKEN = $_COOKIE['gt_admin_token'];
    } else {
      BaseLib::$TOKEN = null;
    }

    if (BaseLib::$TOKEN != null){
      $conn = BaseLib::dbConn();
      try {
        $stmt = $conn->prepare('SELECT * FROM users_tokens WHERE token = ?');
        $stmt->execute(array(BaseLib::$TOKEN));
        if ($row = $stmt->fetch()){
          BaseLib::$ROLE = $row['role'];
          BaseLib::$USERID = $row['userid'];
          if (BaseLib::$ROLE != 'admin'){
            BaseLib::$TOKEN = null;
          }
        }else{
          BaseLib::$TOKEN = null;
        }
      } catch(PDOException $e) {
        BaseLib::$TOKEN = null;
        BaseLib::error(500, $e->getMessage());
      }
    }
    
    if (!BaseLib::isAuthorized()) {
      header("Location: login.php");
      exit;
    }

    if (!BaseLib::isAdmin()) {
      BaseLib::$TOKEN = null;
      header("Location: login.php");
      exit;
    }
  }
  
  static function isAdmin() {
    return BaseLib::$ROLE == 'admin';
  }

  static function createGroupSafe($group) {
    $conn = BaseLib::dbConn();
    $stmt_find = $conn->prepare('SELECT id FROM webdiff_file_groups WHERE `name` = ?;');
    $stmt_find->execute(array($group));
    $ret = -1;
    if ($row = $stmt_find->fetch()) {
      $ret = $row['id'];
    } else {
      $stmt_insert = $conn->prepare('INSERT INTO webdiff_file_groups(`name`) VALUES(?);');
      if ($stmt_insert->execute(array($group))) {
        $ret =  $conn->lastInsertId();
      }
    }
    return $ret;
  }

  static function createVersionSafe($ver) {
    $conn = BaseLib::dbConn();
    $stmt_find = $conn->prepare('SELECT id FROM webdiff_versions WHERE `name` = ?;');
    $stmt_find->execute(array($ver));
    $ret = -1;
    if ($row = $stmt_find->fetch()) {
      $ret = $row['id'];
    } else {
      $stmt_insert = $conn->prepare('INSERT INTO webdiff_versions(`name`, comment) VALUES(?,?);');
      if ($stmt_insert->execute(array($ver, $ver))) {
        $ret =  $conn->lastInsertId();
      }
    }
    return $ret;
  }

  static function createDefineFileSafe($filepath, $filename, $parentid) {
    $conn = BaseLib::dbConn();
    $stmt_find = $conn->prepare('SELECT id FROM webdiff_define_files WHERE filepath = ? AND parent_id = ?;');
    $stmt_find->execute(array($filepath, $parentid));
    $ret = -1;
    if ($row = $stmt_find->fetch()) {
      $ret = $row['id']; // already exists
    } else {
      $stmt_insert = $conn->prepare('INSERT INTO webdiff_define_files(filepath, `filename`, parent_id) VALUES(?,?,?);');
      if ($stmt_insert->execute(array($filepath, $filename, $parentid))) {
        $ret = $conn->lastInsertId();
      }
    }
    return $ret;
  }

  static function createFileSafe($version_id, $group_id, $def_file_id, $file_parent_id) {
    $conn = BaseLib::dbConn();
    $stmt_find = $conn->prepare('SELECT id FROM webdiff_files WHERE version_id = ? AND define_file_id = ? AND file_group_id = ? AND parent_id = ?;');
    $stmt_find->execute(array($version_id, $def_file_id, $group_id, $file_parent_id));
    $ret = -1;
    if ($row = $stmt_find->fetch()) {
      $ret = $row['id'];
    } else {
      $stmt_insert = $conn->prepare('INSERT INTO webdiff_files(version_id, define_file_id, file_group_id, parent_id) VALUES(?,?,?,?);');
      if ($stmt_insert->execute(array($version_id, $def_file_id, $group_id, $file_parent_id))) {
        $ret =  $conn->lastInsertId();
      }
      // update amnount of childs
      $stmt_count = $conn->prepare('SELECT COUNT(id) as cnt FROM webdiff_files WHERE parent_id = ?');
      $stmt_count->execute(array($file_parent_id));
      if ($row = $stmt_count->fetch()) {
        $cnt = $row['cnt'];
        $stmt_update = $conn->prepare('UPDATE webdiff_files SET amount_of_children = ? WHERE id = ?');
        $stmt_update->execute(array($cnt, $file_parent_id));
      }
    }
    return $ret;
  }
}

// load config
include_once ($curdir_gt."/conf.d/config.php");
BaseLib::$CONFIG = $config;

class GTLog {

  static function ok($tag, $msg){
    $conn = BaseLib::dbConn();
    $stmt = $conn->prepare('INSERT INTO events(type, tag, message, created) VALUES(?, ?, ?, NOW())');
    if (!$stmt->execute(array('ok', $tag, $msg))) {
      BaseLib::error(500, $stmt->errorInfo());
    }
  }

  static function info($tag, $msg){
    $conn = BaseLib::dbConn();
    $stmt = $conn->prepare('INSERT INTO events(type, tag, message, created) VALUES(?, ?, ?, NOW())');
    if (!$stmt->execute(array('info', $tag, $msg))) {
      BaseLib::error(500, $stmt->errorInfo());
    }
  }

  static function err($tag, $msg){
    $conn = BaseLib::dbConn();
    error_log('[ERROR] '.$tag.': '.$msg);
    $stmt = $conn->prepare('INSERT INTO events(type, tag, message, created) VALUES(?, ?, ?, NOW())');
    if (!$stmt->execute(array('err', $tag, $msg))) {
      BaseLib::error(500, $stmt->errorInfo());
    }
  }

  static function warn($tag, $msg){
    $conn = BaseLib::dbConn();
    error_log('[WARN] '.$tag.': '.$msg);
    $stmt = $conn->prepare('INSERT INTO events(type, tag, message, created) VALUES(?, ?, ?, NOW())');
    if (!$stmt->execute(array('warn', $tag, $msg))) {
      BaseLib::error(500, $stmt->errorInfo());
    }
  }
}