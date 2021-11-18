<?php

function update0005($conn){
    $stmt = $conn->prepare("
        ALTER TABLE webdiff_define_files ADD `filename` VARCHAR(255) DEFAULT '';
    ");

    if (!$stmt->execute()) {
        error_log(print_r($stmt->errorInfo(),true));
        return false;
    }

    return true;
}