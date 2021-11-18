<?php

function update0003($conn){
    $stmt = $conn->prepare("
        CREATE TABLE `webdiff_files` (
            `id` int(11) NOT NULL AUTO_INCREMENT,
            `version_id` int(11) NOT NULL,
            `define_file_id` int(11) NOT NULL,
            PRIMARY KEY (`id`)
        ) ENGINE=InnoDB DEFAULT CHARSET=utf8 AUTO_INCREMENT=1;
    ");

    if (!$stmt->execute()) {
        error_log(print_r($stmt->errorInfo(),true));
        return false;
    }
    return true;
}