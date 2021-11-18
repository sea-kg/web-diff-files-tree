<?php
      include_once("base_lib.php"); 
      include_once("head.php");
?>
      <div class="container">
      <h1>Registered versions</h1>
        <?php
        $conn = BaseLib::dbConn();

        $stmt = $conn->prepare('SELECT * FROM webdiff_versions ORDER BY `name`');
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
    ?>
      </div>
  </body>
</html>