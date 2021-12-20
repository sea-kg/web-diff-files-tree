#include "models_diff.h"
#include <iostream>

// ModelDiffFile

ModelDiffFile::ModelDiffFile() {
    m_nFileId = -1;
    m_nParentFileId = -1;
    m_nVersionId = -1;
    m_nDefineFileId = -1;
    m_nAmountOfChildren = 0;
    m_nLeftFileSize = 0;
    m_nLeftFileCompressedSize = 0;
    m_nRightFileSize = 0;
    m_nRightFileCompressedSize = 0;
    m_sFilename = "None";
    m_sFilepath = "None";
    m_sState = "None";
}

void ModelDiffFile::setId(int nFileId) {
    m_nFileId = nFileId;
}

int ModelDiffFile::getId() const {
    return m_nFileId;
}

void ModelDiffFile::setParentFileId(int nParentFileId) {
    m_nParentFileId = nParentFileId;
}

int ModelDiffFile::getParentFileId() const {
    return m_nParentFileId;
}

void ModelDiffFile::setVersionId(int nVersionId) {
    m_nVersionId = nVersionId;
}

void ModelDiffFile::setDefineFileId(int nDefineFileId) {
    m_nDefineFileId = nDefineFileId;
}

int ModelDiffFile::getDefineFileId() const {
    return m_nDefineFileId;
}

void ModelDiffFile::setAmountOfChildren(int nAmountOfChildren) {
    m_nAmountOfChildren = nAmountOfChildren;
}

void ModelDiffFile::setFilename(const std::string &sFilename) {
    m_sFilename = sFilename;
}

void ModelDiffFile::setFilepath(const std::string &sFilepath) {
    m_sFilepath = sFilepath;
}

void ModelDiffFile::setState(const std::string &sState) {
    m_sState = sState;
}

const std::string &ModelDiffFile::getState() const {
    return m_sState;
}

void ModelDiffFile::setLeftFileMode(const std::string &sLeftFileMode) {
    m_sLeftFileMode = sLeftFileMode;
}

void ModelDiffFile::setIsDir(bool bIsDir) {
    m_bIsDir = bIsDir;
}

void ModelDiffFile::setLeftFileSize(int nLeftFileSize) {
    m_nLeftFileSize = nLeftFileSize;
}

void ModelDiffFile::setLeftFileCompressedSize(int nLeftFileCompressedSize) {
    m_nLeftFileCompressedSize = nLeftFileCompressedSize;
}

void ModelDiffFile::setLeftFileDateTime(const std::string &sLeftDatetime) {
    m_sLeftDatetime = sLeftDatetime;
}

void ModelDiffFile::setRightFileMode(const std::string &sRightFileMode) {
    m_sRightFileMode = sRightFileMode;
}

void ModelDiffFile::setRightFileSize(int nRightFileSize) {
    m_nRightFileSize = nRightFileSize;
}

void ModelDiffFile::setRightFileCompressedSize(int nRightFileCompressedSize) {
    m_nRightFileCompressedSize = nRightFileCompressedSize;
}

void ModelDiffFile::setRightFileDateTime(const std::string &sRightDatetime) {
    m_sRightDatetime = sRightDatetime;
}

nlohmann::json ModelDiffFile::toJson() const {
    nlohmann::json jsonRet;
    jsonRet["id"] = "id" + std::to_string(m_nFileId);
    jsonRet["parent"] = "id" + std::to_string(m_nParentFileId);
    jsonRet["define_file_id"] = m_nDefineFileId;
    jsonRet["filename"] = m_sFilename;
    jsonRet["amount_of_children"] = m_nAmountOfChildren;
    jsonRet["state"] = m_sState;
    jsonRet["is_dir"] = m_bIsDir;
    jsonRet["left_f_mode"] = m_sLeftFileMode;
    jsonRet["left_f_size"] = m_nLeftFileSize;
    jsonRet["left_f_compressedsize"] = m_nLeftFileCompressedSize;
    jsonRet["left_dt"] = m_sLeftDatetime;
    jsonRet["right_f_mode"] = m_sRightFileMode;
    jsonRet["right_f_size"] = m_nRightFileSize;
    jsonRet["right_f_compressedsize"] = m_nRightFileCompressedSize;
    jsonRet["right_dt"] = m_sRightDatetime;
    jsonRet["comments"] = nlohmann::json::array();
    return jsonRet;
}

// ModelDiffGroup

ModelDiffGroup::ModelDiffGroup() {
    m_group = ModelGroup();
    m_nNewFiles = 0;
    m_nMissingFiles = 0;
}

ModelDiffGroup::ModelDiffGroup(const ModelGroup &group) {
    m_group = group;
    m_nNewFiles = 0;
    m_nMissingFiles = 0;
}

int ModelDiffGroup::size() const {
    return m_vFileDiffs.size();
}

void ModelDiffGroup::addDiffFile(const ModelDiffFile &fileDiff) {
    m_vFileDiffs.push_back(fileDiff);
    m_vFilesIds.push_back(fileDiff.getId());
    m_vParentFilesIds.push_back(fileDiff.getParentFileId());
    if (fileDiff.getState() == "new") {
        m_nNewFiles++;
    }
    if (fileDiff.getState() == "missing") {
        m_nMissingFiles++;
    }
}

const ModelGroup &ModelDiffGroup::getGroup() const {
    return m_group;
}

const std::vector<int> &ModelDiffGroup::getParentFilesIds() const {
    return m_vParentFilesIds;
}

bool ModelDiffGroup::hasFileId(int nFileId) const {
    return std::find(m_vFilesIds.begin(), m_vFilesIds.end(), nFileId) != m_vFilesIds.end();
}

nlohmann::json ModelDiffGroup::toJson() const {
    nlohmann::json jsonRet;
    jsonRet["title"] = m_group.getTitle();
    jsonRet["missing"] = m_nMissingFiles;
    jsonRet["new"] = m_nNewFiles;

    nlohmann::json jsonFiles;
    for (int i = 0; i < m_vFileDiffs.size(); i++) {
        std::string sFileID = "id" + std::to_string(m_vFileDiffs[i].getId());
        jsonFiles[sFileID] = m_vFileDiffs[i].toJson();
    }
    jsonRet["files"] = jsonFiles;
    return jsonRet;
}

// ModelComment

ModelComment::ModelComment() {
    m_nCommentId = -1;
    m_nDefineFileId = -1;
    m_sComment = "";
}

void ModelComment::setId(int nId) {
    m_nCommentId = nId;
}

void ModelComment::setDefineFileId(int nId) {
    m_nDefineFileId = nId;
}

void ModelComment::setComment(const std::string &sTitle) {
    m_sComment = sTitle;
}

nlohmann::json ModelComment::toJson() const {
    nlohmann::json jsonRet;
    jsonRet["id"] = m_nCommentId;
    jsonRet["define_file_id"] = m_nDefineFileId;
    jsonRet["comment"] = m_sComment;
    return jsonRet;
}

// ModelDiffGroups

ModelDiffGroups::ModelDiffGroups() {
}

int ModelDiffGroups::getFilesLength() const {
    int nRet = 0;
    for (auto it = m_mapGroups.begin(); it != m_mapGroups.end(); ++it) {
        nRet += it->second.size();
    }
    return nRet;
}

int ModelDiffGroups::getCommentsLength() const {
    int nRet = 0;
    for (auto it = m_mapComments.begin(); it != m_mapComments.end(); ++it) {
        nRet += it->second.size();
    }
    return nRet;
}

void ModelDiffGroups::addDiffFile(
    const ModelGroup &group,
    const ModelDiffFile &fileDiff
) {
    auto it = m_mapGroups.find(group.getId());
    if (it == m_mapGroups.end()) {
        m_mapGroups[group.getId()] = ModelDiffGroup(group);
    }
    m_mapGroups[group.getId()].addDiffFile(fileDiff);
    int nDefineFileId = fileDiff.getDefineFileId();
    if (std::find(m_vFileDefineIds.begin(), m_vFileDefineIds.end(), nDefineFileId) == m_vFileDefineIds.end()) {
        m_vFileDefineIds.push_back(nDefineFileId);
    }
}

const std::map<int, ModelDiffGroup> &ModelDiffGroups::getGroups() {
    return m_mapGroups;
}

const std::vector<int> &ModelDiffGroups::getFilesDefinesIds() const {
    return m_vFileDefineIds;
}

void ModelDiffGroups::setComments(int nDefineFileId, std::vector<ModelComment> vComments) {
    m_mapComments[nDefineFileId] = vComments;
}

nlohmann::json ModelDiffGroups::toJson() const {
    nlohmann::json jsonRet;
    jsonRet["groups"] = nlohmann::json();
    for (auto it = m_mapGroups.begin(); it != m_mapGroups.end(); ++it) {
        int nGroupId = it->first;
        std::string sGroupId = "group" + std::to_string(nGroupId);
        jsonRet["groups"][sGroupId] = it->second.toJson();
    }
    jsonRet["comments"] = nlohmann::json();
    for (auto it = m_mapComments.begin(); it != m_mapComments.end(); ++it) {
        int nDefineFileId = it->first;
        std::string sDefineFileId = "dfid" + std::to_string(nDefineFileId);
        jsonRet["comments"][sDefineFileId] = nlohmann::json::array();
        const std::vector<ModelComment> &vComments = it->second;
        for (int i = 0; i < vComments.size(); i++) {
            jsonRet["comments"][sDefineFileId].push_back(vComments[i].toJson());
        }
    }
    return jsonRet;
}

