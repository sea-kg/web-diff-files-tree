# web-diff-files-tree

[![Docker Pulls](https://img.shields.io/docker/pulls/sea5kg/web-diff-files-tree.svg)](https://hub.docker.com/r/sea5kg/web-diff-files-tree/) [![Total alerts](https://img.shields.io/lgtm/alerts/g/sea-kg/web-diff-files-tree.svg?logo=lgtm&logoWidth=18)](https://lgtm.com/projects/g/sea-kg/web-diff-files-tree/alerts/) [![Language grade: Python](https://img.shields.io/lgtm/grade/python/g/sea-kg/web-diff-files-tree.svg?logo=lgtm&logoWidth=18)](https://lgtm.com/projects/g/sea-kg/web-diff-files-tree/context:python) [![Language grade: Python](https://img.shields.io/lgtm/grade/cpp/g/sea-kg/web-diff-files-tree.svg?logo=lgtm&logoWidth=18)](https://lgtm.com/projects/g/sea-kg/web-diff-files-tree/context:cpp) [![Language grade: JavaScript](https://img.shields.io/lgtm/grade/javascript/g/sea-kg/web-diff-files-tree.svg?logo=lgtm&logoWidth=18)](https://lgtm.com/projects/g/sea-kg/web-diff-files-tree/context:javascript)

Service which allowed loading filepaths by api. 
After this you can compare different `version` and set comment to files.

![screen](screen.jpg)

[docker](https://hub.docker.com/r/sea5kg/web-diff-files-tree)

## Fast start (via docker-compose)

Just:
```
$ wget -O docker-compose.yml https://raw.githubusercontent.com/sea-kg/web-diff-files-tree/main/docker-compose.yml
$ docker-compose up -d
```

Go look to http://localhost:8071/

for clean:
```
$ docker-compose down
```

Example add data:
```
curl --header "content-type: application/json" \
  --request POST \
  --data '{"group":"product1", "version":"0.0.1", "files"[{"path": "some/file1", "size": 0, "mode": "-rw-rw-r--", "compress_size": 0, "is_dir": false, "dt": "2021-11-18T21:56:08Z"}]}' \
  http://localhost:8071/api/add/
```

## Build (ubuntu)

```
$ sudo apt install \
    cmake \
    git-core \
    pkg-config \
    gcc g++ \
    make \
    libmysqlclient-dev
```

```
./build_simple.sh
```

## Build latest release docker

```
$ docker build --rm --tag sea5kg/web-diff-files-tree:latest -f Dockerfile .
```

## Developing with a docker-compose.develop.yml

```
$ docker-compose -f docker-compose.develop.yml up -d
$ docker exec -it webdifffilestree_develop_server bash
# ./build_simple.sh
# ./web-diff-files-tree
```

Cleanup all:
```
$ sudo rm -rf mysql_data
$ sudo rm -rf tmp
$ docker-compose -f docker-compose.develop.yml down
```


## Database creation (manual)

Create empty database via console client for mysql:

```
$ mysql
> CREATE DATABASE `webdiff` CHARACTER SET utf8 COLLATE utf8_general_ci;
> CREATE USER 'webdiffu'@'localhost' IDENTIFIED BY 'jET3E4W9vm';
> GRANT ALL PRIVILEGES ON webdiff.* TO 'webdiffu'@'localhost' WITH GRANT OPTION;
> FLUSH PRIVILEGES;
```
