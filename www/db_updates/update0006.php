<?php

function update0006($conn){
    $stmt = $conn->prepare("
        ALTER TABLE webdiff_files ADD file_group_id int(11) DEFAULT 0;
    ");

    if (!$stmt->execute()) {
        error_log(print_r($stmt->errorInfo(),true));
        return false;
    }

    return true;
}