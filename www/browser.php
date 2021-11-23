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
      <h1>Browser</h1>
        <?php
        $conn = BaseLib::dbConn();

        $stmt = $conn->prepare('SELECT * FROM webdiff_versions ORDER BY `name`');
        $stmt->execute(array());
        while ($row = $stmt->fetch()) {
            $verid = $row['id'];
            $name = $row['name'];
            $elid = 'version_'.$verid;
            echo '<div class="treeitem">
              <div class="treeitemname">
                <div class="itemversions plus" toggleid="'.$elid.'"></div>
                <img width=25px height=25px src="./images/ver.svg">'.$name.'</div>
              <div class="subtree" id="'.$elid.'" version_id="'.$verid.'"></div>
            </div>';
        }
    ?>
      </div>
      <script>
        function rebindClick() {
          // expand versions
          $('.itemversions').unbind('click').bind('click', function(el) {
            var elid = $(this).attr('toggleid');
            var subel = $('#' + elid);
            if (subel.html() != "") {
              subel.html("");
              return;
            }
            var version_id = subel.attr('version_id');
            $.ajax({ url: "./api/groups/", type: 'post', cache: false, dataType: 'json',
              data: JSON.stringify( { "version_id": version_id } ),
              async:true,
            }).fail(function(error) {
              console.log(error);
            }).done(function(result) {
              var _subhtml = '';
              for(var i in result['list']) {
                var item = result['list'][i];
                var group_id = item['group_id'];
                var amount_of_files = item['amount_of_files'];
                var _elid = 'group' + version_id + '_' + group_id;
                _subhtml += '<div class="treeitem">';
                _subhtml += '<div class="treeitemname">';
                _subhtml += '<div class="itemfiles plus" toggleid="' + _elid + '"></div>';
                _subhtml += '<img width=25px height=25px src="./images/group.svg">' + item['title'] + ' (' + amount_of_files + ' files)';
                _subhtml += '</div>'
                _subhtml += '<div class="subtree" id="' + _elid + '" version_id="' + version_id + '" group_id="' + group_id + '" parent_id="0"></div>';
                _subhtml += '</div>';
              }
              subel.html(_subhtml);
              rebindClick();
            })
          })

          // expand files
          $('.itemfiles').unbind('click').bind('click', function(el) {
            var elid = $(this).attr('toggleid');
            var subel = $('#' + elid);
            if (subel.html() != "") {
              subel.html("");
              $(this).removeClass("opened");
              return;
            }
            $(this).addClass("opened");
            var version_id = subel.attr('version_id');
            var group_id = subel.attr('group_id');
            var parent_id = subel.attr('parent_id');
            $.ajax({ url: "./api/files/", type: 'post', cache: false, dataType: 'json',
              data: JSON.stringify( { "version_id": version_id, "group_id": group_id, "parent_id": parent_id } ),
              async: true,
            }).fail(function(error) {
              console.log(error);
            }).done(function(result) {
              var _subhtml = '';
              for(var i in result['list']) {
                var item = result['list'][i];
                var file_id = item['file_id'];
                var def_file_id = item['def_file_id'];
                var amount_of_children = item['amount_of_children'];
                var img_name = amount_of_children > 0 ? 'directory.svg' : 'file.svg';

                var _elid = 'group' + version_id + '_' + group_id + '_' + file_id;
                _subhtml += '<div class="treeitem">';
                _subhtml += '<div class="treeitemname">';
                if (amount_of_children > 0) {
                  _subhtml += '<div class="itemfiles plus" toggleid="' + _elid + '"></div>';  
                } else {
                  _subhtml += '<div class="subfile"></div>';  
                }
                _subhtml += '<img width=25px height=25px src="./images/' + img_name + '">' + item['title'];
                _subhtml += '</div>';
                _subhtml += '<div class="subtree" id="' + _elid + '" version_id="' + version_id + '" group_id="' + group_id + '" parent_id="' + file_id + '" ></div>';
                _subhtml += '</div>';
              }
              subel.html(_subhtml);
              rebindClick();
            })
          })
        }
        rebindClick();
      </script>
  </body>
</html>