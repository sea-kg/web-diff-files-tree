<?php
  include_once("base_lib.php"); 
  include_once("head.php");

  $conn = BaseLib::dbConn();
  $stmt = $conn->prepare('SELECT * FROM webdiff_versions ORDER BY `name`');
  $stmt->execute(array());
  $options_left_version = '';
  $options_right_version = '';
  $left_version = -1;
  $right_version = -1;
  if (isset($_GET['left_version']) && isset($_GET['right_version'])) {
    $left_version = $_GET['left_version'];
    $right_version = $_GET['right_version'];
  }
  while ($row = $stmt->fetch()) {
    $verid = $row['id'];
    $name = $row['name'];

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
    <?php } ?>
  </div>
  <script>
  </script>
</body>
</html>