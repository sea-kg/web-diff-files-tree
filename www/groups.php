<!doctype html>
<html lang="ru">
  <head>
    <meta charset="utf-8">
    <meta name="viewport" content="width=device-width, initial-scale=1, shrink-to-fit=no">
    
    <script src="./js/jquery-3.4.1.min.js"></script>
    <script src="./js/popper.min.js"></script>
    <script src="./js/bootstrap.min.js"></script>
    <link rel="stylesheet" href="./css/bootstrap.min.css">
    <link rel="stylesheet" href="./css/index.css?v=1">
    <style>
body {
  height: 100%;
}
    </style>

    <title>web-diff-files-tree</title>
  </head>
  <body>
      <nav class="navbar navbar-expand-lg navbar-light bg-light">
          <a class="navbar-brand" href="#">web-diff-files-tree</a>
          <button class="navbar-toggler" type="button" data-toggle="collapse" data-target="#navbarSupportedContent" aria-controls="navbarSupportedContent" aria-expanded="false" aria-label="Toggle navigation">
            <span class="navbar-toggler-icon"></span>
          </button>
        
          <div class="collapse navbar-collapse" id="navbarSupportedContent">
            <ul class="navbar-nav mr-auto">
              <li class="nav-item active">
                <a class="nav-link" href="browser.php">Browser</a>
              </li>
              <li class="nav-item active">
                <a class="nav-link" href="index.php">Diff</a>
              </li>
              <li class="nav-item active">
                <a class="nav-link" href="groups.php">Groups</a>
              </li>
            </ul>
          </div>
        </nav>

<?php
      include_once("base_lib.php"); 
?>
      <div class="container">
      <h1>Registered file groups</h1>
        <?php
        $conn = BaseLib::dbConn();

        $stmt = $conn->prepare('SELECT * FROM webdiff_file_groups ORDER BY `name`');
        $stmt->execute(array());
        while ($row = $stmt->fetch()) {
            $verid = $row['id'];
            $name = $row['name'];

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