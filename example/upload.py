#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import zipfile
import os
import sys
import re
import stat
import requests
import datetime

# BASEURL = "http://localhost:8071/"
BASEURL = "http://localhost:1234"

def get_group_and_version(_zip_filename):
    _zip_filename = _zip_filename[:-4] # remove '.zip'
    _items = _zip_filename.split("_")
    match = re.search(r'\d+\.\d+.*',_zip_filename)
    if match is None:
        sys.exit("Could not extract version info from filename")
    version = match.group()
    group = _zip_filename.split(version)[0]
    if group[len(group) - 1] == '_':
        group = group[:-1]
    return (group, version)

def remove_first_folder(_file):
    _file = _file.split("/")
    _file = _file[1:]
    return "/".join(_file)

def upload_files(data_request, _uploaded, _all):
    print(" ------- ")
    print("Start adding information. Group: " + data_request['group'] + "; Version: " + data_request['version'])
    print("Uploading " + str(len(data_request['files'])) + "....")
    r = requests.post(BASEURL + '/api/add/', json = data_request)
    if r.status_code != 200:
        sys.exit("Could not post " + str(r.status_code ) + r.content)
    _uploaded += len(data_request['files'])
    print("Uploaded " + str(_uploaded) + "/" + str(_all))
    return _uploaded

def getfileinfo(_zip, _reg_file):
    ret = {}
    fi = _zip.getinfo(_reg_file)
    ret["name"] = remove_first_folder(_reg_file)
    ret["size"] = fi.file_size
    ret["mode"] = stat.filemode(fi.external_attr >> 16)
    ret["compress_size"] = fi.compress_size
    ret["is_dir"] = fi.is_dir()
    dt = datetime.datetime(
        fi.date_time[0],
        fi.date_time[1],
        fi.date_time[2],
        fi.date_time[3],
        fi.date_time[4],
        fi.date_time[5]
    )
    ret["dt"] = dt.strftime("%Y-%m-%dT%H:%M:%SZ")
    print(ret)
    return ret

files = os.listdir(".")
for _zip_filename in files:
    if not _zip_filename.endswith(".zip"):
        continue
    data_request = {}
    (group, version) = get_group_and_version(_zip_filename)
    data_request['group'] = group
    data_request['version'] = version
    data_request['files'] = []

    _zip = zipfile.ZipFile(_zip_filename)
    reg_files = _zip.namelist()
    # print(reg_files)
    # for f in reg_files:
    #     # https://docs.python.org/3/library/zipfile.html#zipinfo-objects
    #     fi = _zip.getinfo(f)
    #     filemode = stat.filemode(fi.external_attr >> 16)
    #     print(
    #         f + " size: "
    #         + str(fi.file_size)
    #         + ", compress_size: "
    #         + str(fi.compress_size)
    #         + ", is_dir: "
    #         + str(fi.is_dir())
    #         + ", date_time: "
    #         + str(fi.date_time)
    #         + ", filemode: "
    #         + str(filemode)
    #     )

    _all = len(reg_files)
    _uploaded = 0
    print(" ------- ")
    print("Start adding information. Group: " + data_request['group'] + "; Version: " + data_request['version'])
    for _reg_file in reg_files:
        data_request['files'].append(getfileinfo(_zip, _reg_file))
        if len(data_request['files']) > 499:
            _uploaded = upload_files(data_request, _uploaded, _all)
            data_request['files'] = []
    if len(data_request['files']) > 0:
        _uploaded = upload_files(data_request, _uploaded, _all)
        _uploaded += len(data_request['files'])
        data_request['files'] = []
    print("Done.")
