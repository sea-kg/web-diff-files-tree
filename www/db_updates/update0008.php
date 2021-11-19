<?php

function update0008($conn){
    $stmt = $conn->prepare("
        ALTER TABLE webdiff_files ADD parent_id int(11) DEFAULT 0;
    ");

    if (!$stmt->execute()) {
        error_log(print_r($stmt->errorInfo(),true));
        return false;
    }

    $stmt = $conn->prepare("
        ALTER TABLE webdiff_files ADD amount_of_children int(11) DEFAULT 0;
    ");

    if (!$stmt->execute()) {
        error_log(print_r($stmt->errorInfo(),true));
        return false;
    }

    return true;
}