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
            $elid = 'ver_'.$verid;
            echo '<div class="treeitem">
              <div class="treeitemname" toggleid="'.$elid.'"><img width=25px height=25px src="./images/ver.svg">'.$name.'</div>
              <div class="subtree" id="'.$elid.'" load="groups" byid="'.$verid.'"></div>
            </div>';
        }
    ?>
      </div>
      <script>
        function rebindClick() {
          $('.treeitemname').unbind('click').bind('click', function(el) {
            var elid = $(this).attr('toggleid');
            var subel = $('#' + elid);
            if (subel.html() != "") {
              subel.html("");
              return;
            }
            var load = subel.attr('load');
            var byid = subel.attr('byid');

            console.log(elid);
            console.log(load);
            console.log(byid);
            console.log(el);
            $.ajax({
              url: "./api/",
              type: 'post',
              cache: false,
              dataType: 'json',
              data: JSON.stringify( { "action": "loadtree", "load": load, "byid": byid } ),
              async:true,
            }).done(function(result) {
              var _subhtml = '';
              var load1 = result['load'];
              var byid1 = result['byid'];
              for(var i in result['list']) {
                var item = result['list'][i];
                var _elid = load1 + item['child_id'];
                _subhtml += '<div class="treeitem">';
                _subhtml += '<div class="treeitemname" toggleid="' + _elid + '"><img width=25px height=25px src="./images/' + load1 + '.svg">' + item['title'] + '</div>'
                _subhtml += '<div class="subtree" id="' + _elid + '" load="files" byid="' + byid1 + ':' + item['child_id'] + '"></div>'
                _subhtml += '</div>';
              }
              subel.html(_subhtml);
              rebindClick();
            }).fail(function(error) {
              console.error(error)
            });

          })
        }
        rebindClick();
      </script>
  </body>
</html>