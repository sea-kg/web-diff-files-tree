#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import zipfile
import os
import requests

BASEURL = "http://localhost/webdiff/"



def get_file_group(_zip_filename):
    # here you can modify code for extract from 
    return _zip_filename.split("_")[0]

def get_version(_zip_filename):
    _zip_filename = _zip_filename[:-4] # remove '.zip'
    _items = _zip_filename.split("_")
    return _items[1]

def remove_first_folder(_file):
    _file = _file.split("/")
    _file = _file[1:]
    return "/".join(_file)

files = os.listdir(".")
for _zip_filename in files:
    if not _zip_filename.endswith(".zip"):
        continue
    data_request = {}
    data_request['action'] = 'add'
    data_request['group'] = get_file_group(_zip_filename)
    data_request['version'] = get_version(_zip_filename)
    data_request['files'] = []

    zip = zipfile.ZipFile(_zip_filename)
    reg_files = zip.namelist()
    _all = len(reg_files)
    _uploaded = 0
    print(" ------- ")
    print("Start adding infoemation. Group: " + data_request['group'] + "; Version: " + data_request['version'])
    for _reg_file in reg_files:
        _reg_file = remove_first_folder(_reg_file)
        data_request['files'].append(_reg_file)
        if len(data_request['files']) > 499:
            print("Uploading " + str(len(data_request['files'])) + "....")
            r = requests.post(BASEURL + '/api/', json = data_request)
            _uploaded += len(data_request['files'])
            print("Uploaded " + str(_uploaded) + "/" + str(_all))
            data_request['files'] = []
    if len(data_request['files']) > 0:
        r = requests.post(BASEURL + '/api/', json = data_request)
        _uploaded += len(data_request['files'])
        print("Uploaded " + str(_uploaded) + "/" + str(_all))
        data_request['files'] = []
    print("Done.")
