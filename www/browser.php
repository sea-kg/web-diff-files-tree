<?php
      include_once("base_lib.php"); 
      include_once("head.php");
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
              <div class="treeitemname itemversions" toggleid="'.$elid.'"><img width=25px height=25px src="./images/ver.svg">'.$name.'</div>
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
                var _elid = 'group' + version_id + '_' + group_id;
                _subhtml += '<div class="treeitem">';
                _subhtml += '<div class="treeitemname itemfiles" toggleid="' + _elid + '"><img width=25px height=25px src="./images/group.svg">' + item['title'] + '</div>'
                _subhtml += '<div class="subtree" id="' + _elid + '" version_id="' + version_id + '" group_id="' + group_id + '" parent_id="0"></div>'
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
              return;
            }
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
                  _subhtml += '<div class="treeitemname itemfiles" toggleid="' + _elid + '"><img width=25px height=25px src="./images/' + img_name + '">' + item['title'] + '</div>'
                  _subhtml += '<div class="subtree" id="' + _elid + '" version_id="' + version_id + '" group_id="' + group_id + '" parent_id="' + file_id + '" ></div>'
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