#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import zipfile
import os
import sys
import re
import requests

# BASEURL = "http://localhost:8071/"
BASEURL = "http://localhost/webdiff/"

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

files = os.listdir(".")
for _zip_filename in files:
    if not _zip_filename.endswith(".zip"):
        continue
    data_request = {}
    (group, version) = get_group_and_version(_zip_filename)
    data_request['group'] = group
    data_request['version'] = version
    data_request['files'] = []

    zip = zipfile.ZipFile(_zip_filename)
    reg_files = zip.namelist()
    _all = len(reg_files)
    _uploaded = 0
    print(" ------- ")
    print("Start adding information. Group: " + data_request['group'] + "; Version: " + data_request['version'])
    for _reg_file in reg_files:
        _reg_file = remove_first_folder(_reg_file)
        data_request['files'].append(_reg_file)
        if len(data_request['files']) > 499:
            print("Uploading " + str(len(data_request['files'])) + "....")
            r = requests.post(BASEURL + '/api/add/', json = data_request)
            if r.status_code != 200:
                sys.exit("Could not post")
            _uploaded += len(data_request['files'])
            print("Uploaded " + str(_uploaded) + "/" + str(_all))
            data_request['files'] = []
    if len(data_request['files']) > 0:
        r = requests.post(BASEURL + '/api/add/', json = data_request)
        if r.status_code != 200:
            sys.exit("Could not post")
        _uploaded += len(data_request['files'])
        print("Uploaded " + str(_uploaded) + "/" + str(_all))
        data_request['files'] = []
    print("Done.")
