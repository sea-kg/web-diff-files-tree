<?php
  include_once("base_lib.php"); 
  include_once("head.php");

  $conn = BaseLib::dbConn();
  $stmt = $conn->prepare('SELECT * FROM webdiff_versions ORDER BY `name`');
  $stmt->execute(array());
  $options_left_version = '';
  $options_right_version = '';
  $left_version = -1;
  $left_version_name = "";
  $right_version = -1;
  if (isset($_GET['left_version']) && isset($_GET['right_version'])) {
    $left_version = $_GET['left_version'];
    $right_version = $_GET['right_version'];
  }
  while ($row = $stmt->fetch()) {
    $verid = $row['id'];
    $name = $row['name'];
    if ($verid == $left_version) {
      $left_version_name = $name;
    }

    if ($verid == $right_version) {
      $right_version_name = $name;
    }

    $options_left_version .= "<option ".($left_version==$verid ? "selected" : "")." value='$verid'>".htmlspecialchars($name)." (#$verid)</option>";
    $options_right_version .= "<option ".($right_version==$verid ? "selected" : "")." value='$verid'>".htmlspecialchars($name)." (#$verid)</option>";
  }
?>
  <br>
  <div class="container">
    <?php if ($left_version == -1 && $right_version == -1) { ?>
      Compare version:
      <form>
        <select class="form-select" name="left_version">
          <?php echo $options_left_version; ?>
        </select>
        With this version:
        <select class="form-select" name="right_version">
          <?php echo $options_right_version; ?>
        </select>
        <button class="btn btn-primary" type="submit">Start compare</button>
      </form>
    <?php } else { ?>
      <form>
        <input type="hidden" name="left_version" value="-1"/>
        <input type="hidden" name="right_version" value="-1"/>
        <button class="btn btn-primary" type="submit">Other comparation</button>
      </form>
    <?php
      $conn = BaseLib::dbConn();
      // SELECT * FROM webdiff_files WHERE version_id = 4 AND define_file_id NOT IN (SELECT define_file_id FROM webdiff_files WHERE version_id = 5);
      echo "<h2>Missing files in #".$right_version_name.", but exists in ".$left_version_name."</h2>";
      $stmt = $conn->prepare('
        SELECT * FROM webdiff_files
        INNER JOIN webdiff_define_files t1 ON t1.id = define_file_id
        INNER JOIN webdiff_file_groups t2 ON t2.id = file_group_id
        WHERE version_id = ? AND define_file_id NOT IN (SELECT define_file_id FROM webdiff_files WHERE version_id = ?);');
      $stmt->execute(array($left_version, $right_version));
      while ($row = $stmt->fetch()) {
          $define_file_id = $row['define_file_id'];
          $filepath = $row['filepath'];
          echo "#".$define_file_id." ".$filepath." <br>";
          // print_r($row);
          // echo $define_file_id;
      }

      echo "<h2>New files in #".$right_version_name." and not exists in ".$left_version_name."</h2>";
      $stmt = $conn->prepare('
        SELECT * FROM webdiff_files
        INNER JOIN webdiff_define_files t1 ON t1.id = define_file_id
        WHERE version_id = ? AND define_file_id NOT IN (SELECT define_file_id FROM webdiff_files WHERE version_id = ?);');
      $stmt->execute(array($right_version, $left_version));
      while ($row = $stmt->fetch()) {
          $define_file_id = $row['define_file_id'];
          $filepath = $row['filepath'];
          echo "#".$define_file_id." ".$filepath." <br>";
          // print_r($row);
          // echo $define_file_id;
      }

      // if selected versions
      } ?>
  </div>
  <script>
  </script>
</body>
</html>
