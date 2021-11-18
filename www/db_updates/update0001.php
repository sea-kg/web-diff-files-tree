<?php

function update0001($conn){
    $stmt = $conn->prepare("
        CREATE TABLE webdiff_db_updates (
            `id` int(11) NOT NULL AUTO_INCREMENT,
            `version` int(11) NOT NULL,
            `dt` datetime NOT NULL,
            PRIMARY KEY (`id`)
        ) ENGINE=InnoDB DEFAULT CHARSET=utf8 AUTO_INCREMENT=1;
    ");
    
    if (!$stmt->execute()) {
        error_log(print_r($stmt->errorInfo(),true));
        return false;
    }
    
    $stmt = $conn->prepare("
        CREATE TABLE `webdiff_users` (
            `id` int(11) NOT NULL AUTO_INCREMENT,
            `username` varchar(255) NOT NULL,
            `userpass` varchar(100) NOT NULL,
            `role` varchar(10) NOT NULL,
            `comment` varchar(1024) NOT NULL,
            `created` datetime NOT NULL DEFAULT CURRENT_TIMESTAMP,
            PRIMARY KEY (`id`),
            UNIQUE(`username`)
        ) ENGINE=InnoDB DEFAULT CHARSET=utf8, AUTO_INCREMENT=1;
    ");
    
    if (!$stmt->execute()) {
        error_log(print_r($stmt->errorInfo(),true));
        return false;
    }

    $stmt = $conn->prepare("INSERT INTO webdiff_users(username,userpass,role,comment) VALUES(?,?,?,?)");
    
    if (!$stmt->execute(array('admin','d033e22ae348aeb5660fc2140aec35850c4da997','admin', ''))) {
        error_log(print_r($stmt->errorInfo(),true));
        return false;
    }

    $stmt = $conn->prepare("
        CREATE TABLE `webdiff_users_tokens` (
            `id` int(11) NOT NULL AUTO_INCREMENT,
            `token` varchar(255) NOT NULL,
            `role` varchar(10) NOT NULL,
            `userid` int(11) NOT NULL,
            `created` datetime NOT NULL DEFAULT CURRENT_TIMESTAMP,
            PRIMARY KEY (`id`),
            UNIQUE(`token`)
        ) ENGINE=InnoDB DEFAULT CHARSET=utf8, AUTO_INCREMENT=1;
    ");

    if (!$stmt->execute()) {
        error_log(print_r($stmt->errorInfo(),true));
        return false;
    }

    $stmt = $conn->prepare("
        CREATE TABLE `webdiff_versions` (
            `id` int(11) NOT NULL AUTO_INCREMENT,
            `name` varchar(255) NOT NULL,
            `comment` varchar(255) NOT NULL,
            PRIMARY KEY (`id`)
        ) ENGINE=InnoDB DEFAULT CHARSET=utf8 AUTO_INCREMENT=1;
    ");

    if (!$stmt->execute()) {
        error_log(print_r($stmt->errorInfo(),true));
        return false;
    }

    return true;
}