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
  $right_version_name = "";
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
      <div id="diff_content">

      </div>

      <?php
        echo '<script>
          var left_version_id = '.$left_version.';
          var right_version_id = '.$right_version.';
          var left_version_name = "'.$left_version_name.'";
          var right_version_name = "'.$right_version_name.'";
        </script>
        ';
      ?>
      <script>
        function rebindClick() {
          // expand files
          $('.itemfiles').unbind('click').bind('click', function(el) {
            var elid = $(this).attr('toggleid');
            var subel = $('#' + elid);
            if ($(this).hasClass("opened")) {
              $(this).removeClass("opened");
              subel.hide();
            } else {
              $(this).addClass("opened");
              subel.show();
            }
          })


          $('.comment-add').unbind('click').bind('click', function(el) {
            var define_file_id = $(this).attr('define_file_id');
            var comment = prompt("New comment:");
            
            if (comment) {
              $.ajax({ url: "./api/comment-add/", type: 'post', cache: false, dataType: 'json',
                data: JSON.stringify( { "define_file_id": define_file_id, "comment": comment, } ),
                async:true,
              }).fail(function(error) {
                console.log(error);
              }).done(function(result) {
                $('.comments[define_file_id=' + define_file_id + ']').prepend(
                  '<div class="comment" comment_id="' + result['id'] + '">'
                  + '<div class="comment-text">(c#' + result['id'] + ') ' + result['comment'] + '</div>'
                  + '<div class="comment-hide" comment_id="' + result['id'] + '"></div>'
                  + '</div>'
                )
              })
              rebindClick();
            }
          })

          $('.comment-hide').unbind('click').bind('click', function(el) {
            var comment_id = $(this).attr('comment_id');
            // var comment = prompt("New comment:");
            
            // if (comment) {
              $.ajax({ url: "./api/comment-hide/", type: 'post', cache: false, dataType: 'json',
                data: JSON.stringify( { "comment_id": comment_id } ),
                async:true,
              }).fail(function(error) {
                console.log(error);
              }).done(function(result) {
                $('.comment[comment_id=' + comment_id + ']').remove();
              })
              rebindClick();
            // }
          })
        }

        function createsubtree(files, startid) {
          var _subhtml = '';
          for (var fid in files) {
            var fileinfo = files[fid];
            if (fileinfo['parent'] == startid) {
              var amount_of_children = fileinfo['amount_of_children'];
              var define_file_id = fileinfo['define_file_id'];
              var img_name = amount_of_children > 0 ? 'directory.svg' : 'file.svg';
              
              var _elid = 'file_' + fid;
              _subhtml += '<div class="treeitem">';
              _subhtml += '<div class="treeitemname">';
              if (amount_of_children > 0) {
                _subhtml += '<div class="itemfiles plus opened" toggleid="' + _elid + '"></div>';  
              } else {
                _subhtml += '<div class="subfile"></div>';  
              }
              _subhtml += '<img width=25px height=25px src="./images/' + img_name + '">' + fileinfo['filename'];
              if (fileinfo['state'] == 'new') {
                _subhtml += '<div class="state new">NEW in ' + right_version_name + ', but not found in ' + left_version_name + ' </div>';
                _subhtml += '<div class="comment-add" define_file_id="' + define_file_id + '" ></div>'
                _subhtml += '<div class="file-check" file_id="' + fid + '" ></div>'
              } else if (fileinfo['state'] == 'missing') {
                _subhtml += '<div class="state missing">MISSING in ' + right_version_name + ', but exists in ' + left_version_name + ' </div>';
                _subhtml += '<div class="comment-add" define_file_id="' + define_file_id + '" ></div>'
                _subhtml += '<div class="file-check" file_id="' + fid + '" ></div>'
              }
              _subhtml += '<div class="comments" define_file_id="' + define_file_id + '">';
              if (fileinfo['comments'] && fileinfo['comments'].length > 0) {
                for (var cm in fileinfo['comments']) {
                  cm = fileinfo['comments'][cm];
                  _subhtml += '<div class="comment" comment_id="' + cm['id'] + '">'
                    + '<div class="comment-text">(c#' + cm['id'] + ') ' + cm['comment'] + '</div>'
                    + '<div class="comment-hide" comment_id="' + cm['id'] + '"></div>'
                    + '</div>';
                }
              }
              _subhtml += '</div>';

              _subhtml += '</div>';
              _subhtml += '<div class="subtree" id="' + _elid + '">' + createsubtree(files, fid) + '</div>';
              _subhtml += '</div>';
            }
          }
          return _subhtml
        }

        function loadDiff() {
          $.ajax({ url: "./api/diff/", type: 'post', cache: false, dataType: 'json',
            data: JSON.stringify( { "left_version_id": left_version_id, "right_version_id": right_version_id, } ),
            async:true,
          }).fail(function(error) {
            console.log(error);
          }).done(function(result) {
            document.diff_result = result;
            console.log(result);
            var groups = result['groups'];
            var content = $('#diff_content');
            content.append('<h1>Result of comparation</h1>');
            for (var groupid in groups) {
              var _elid = 'group' + groupid;
              var group = groups[groupid];
              content.append(''
                + '<div class="treeitem">'
                + '<div class="treeitemname">'
                + '<div class="itemfiles plus opened" toggleid="' + _elid + '"></div>'
                + '<img width=25px height=25px src="./images/group.svg">' + group['title'] + ' (new files: ' + group['new']  + ', missing files: ' + group['missing'] + ')'
                + '</div>'
                + '<div class="subtree" id="' + _elid + '">' + createsubtree(group['files'], 'id0') + '</div>'
                + '</div>'
              );
            }
            rebindClick();
          })
        }
        loadDiff();
      </script>


    <?php
      // if selected versions
      } ?>
  </div>
  <script>
  </script>
</body>
</html>
