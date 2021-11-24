<?php

$webdiff_db_host = "localhost";

if (getenv("WEBDIFF_DB_HOST") !== false) {
    $webdiff_db_host = getenv("WEBDIFF_DB_HOST");
}

$config = array();
$config['conn'] = array(
    "host" => $webdiff_db_host,
    "db" => "webdiff",
    "username" => "webdiffu",
    "password" => "jET3E4W9vm",
);
