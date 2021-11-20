<?php

function update0009($conn){
    $stmt = $conn->prepare("
        ALTER TABLE webdiff_define_files_comments ADD hided int(11) DEFAULT 0;
    ");

    if (!$stmt->execute()) {
        error_log(print_r($stmt->errorInfo(),true));
        return false;
    }

    return true;
}