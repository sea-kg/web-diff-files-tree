#include "model_diff_groups.h"
#include <iostream>

// ModelDiffFile

ModelDiffFile::ModelDiffFile() {
    m_nFileId = -1;
    m_nParentFileId = -1;
    m_nVersionId = -1;
    m_nDefineFileId = -1;
    m_nAmountOfChildren = 0;
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

nlohmann::json ModelDiffFile::toJson() const {
    nlohmann::json jsonRet;
    jsonRet["id"] = "id" + std::to_string(m_nFileId);
    jsonRet["parent"] = "id" + std::to_string(m_nParentFileId);
    jsonRet["define_file_id"] = m_nDefineFileId;
    jsonRet["filename"] = m_sFilename;
    jsonRet["amount_of_children"] = m_nAmountOfChildren;
    jsonRet["state"] = m_sState;
    jsonRet["comments"] = nlohmann::json::array();
    // jsonRet["comments"] => load_comments($conn, intval($define_file_id)),
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

const std::vector<int> &ModelDiffGroup::getFilesIds() const {
    return m_vFilesIds;
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

// ModelDiffGroups

ModelDiffGroups::ModelDiffGroups() {
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
}

const std::map<int, ModelDiffGroup> &ModelDiffGroups::getGroups() {
    return m_mapGroups;
}

nlohmann::json ModelDiffGroups::toJson() const {
    nlohmann::json jsonRet;
    for (auto it = m_mapGroups.begin(); it != m_mapGroups.end(); ++it) {
        int nGroupId = it->first;
        std::string sGroupId = "group" + std::to_string(nGroupId);
        jsonRet[sGroupId] = it->second.toJson();
    }
    return jsonRet;
}
