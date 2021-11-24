<?php

function update0007($conn){
    $stmt = $conn->prepare("
        ALTER TABLE webdiff_define_files DROP COLUMN file_group_id;
    ");

    if (!$stmt->execute()) {
        error_log(print_r($stmt->errorInfo(),true));
        return false;
    }

    return true;
}