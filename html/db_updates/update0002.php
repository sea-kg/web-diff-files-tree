<?php

function update0002($conn){

    $stmt = $conn->prepare("
        CREATE TABLE `webdiff_file_groups` (
            `id` int(11) NOT NULL AUTO_INCREMENT,
            `name` varchar(255) NOT NULL,
            PRIMARY KEY (`id`)
        ) ENGINE=InnoDB DEFAULT CHARSET=utf8 AUTO_INCREMENT=1;
    ");

    if (!$stmt->execute()) {
        error_log(print_r($stmt->errorInfo(),true));
        return false;
    }

    $stmt = $conn->prepare("
        CREATE TABLE `webdiff_define_files` (
            `id` int(11) NOT NULL AUTO_INCREMENT,
            `file_group_id` int(11) NOT NULL,
            `filepath` varchar(4096) NOT NULL,
            PRIMARY KEY (`id`)
        ) ENGINE=InnoDB DEFAULT CHARSET=utf8 AUTO_INCREMENT=1;
    ");

    if (!$stmt->execute()) {
        error_log(print_r($stmt->errorInfo(),true));
        return false;
    }

    $stmt = $conn->prepare("
        CREATE TABLE `webdiff_define_files_comments` (
            `id` int(11) NOT NULL AUTO_INCREMENT,
            `define_file_id` int(11) NOT NULL,
            `comment` TEXT NOT NULL,
            `dt_created` datetime NOT NULL,
            PRIMARY KEY (`id`)
        ) ENGINE=InnoDB DEFAULT CHARSET=utf8 AUTO_INCREMENT=1;
    ");

    if (!$stmt->execute()) {
        error_log(print_r($stmt->errorInfo(),true));
        return false;
    }

    return true;
}