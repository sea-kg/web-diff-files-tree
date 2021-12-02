
#include "models_basic.h"

// ModelVersion

ModelVersion::ModelVersion() {
    m_nId = -1;
    m_sName = "None";
}

void ModelVersion::setId(int nId) {
    m_nId = nId;
}

int ModelVersion::getId() {
    return m_nId;
}

void ModelVersion::setName(const std::string &sName) {
    m_sName = sName;
}

nlohmann::json ModelVersion::toJson() const {
    nlohmann::json jsonRet;
    jsonRet["version_id"] = m_nId;
    jsonRet["name"] = m_sName;
    return jsonRet;
}

// ModelGroup

ModelGroup::ModelGroup() {
    m_nId = -1;
    m_sTitle = "None";
}

void ModelGroup::setId(int nId) {
    m_nId = nId;
}

int ModelGroup::getId() const {
    return m_nId;
}

void ModelGroup::setTitle(const std::string &sTitle) {
    m_sTitle = sTitle;
}

const std::string &ModelGroup::getTitle() const {
    return m_sTitle;
}

nlohmann::json ModelGroup::toJson() const {
    nlohmann::json jsonRet;
    jsonRet["group_id"] = m_nId;
    jsonRet["name"] = m_sTitle;
    return jsonRet;
}

// ModelGroupForVersion

ModelGroupForVersion::ModelGroupForVersion() {
    m_nGroupId = -1;
    m_nVersionId = -1;
    m_sTitle = "None";
    m_nAmountOfFiles = 0;
}

void ModelGroupForVersion::setId(int nId) {
    m_nGroupId = nId;
}

void ModelGroupForVersion::setVersionId(int nId) {
    m_nVersionId = nId;
}

void ModelGroupForVersion::setTitle(const std::string &sTitle) {
    m_sTitle = sTitle;
}

void ModelGroupForVersion::setAmountOfFiles(int nAmountOfFiles) {
    m_nAmountOfFiles = nAmountOfFiles;
}

nlohmann::json ModelGroupForVersion::toJson() const {
    nlohmann::json jsonRet;
    jsonRet["version_id"] = m_nVersionId;
    jsonRet["group_id"] = m_nGroupId;
    jsonRet["title"] = m_sTitle;
    jsonRet["amount_of_files"] = m_nAmountOfFiles;
    return jsonRet;
}

// ModelFile

ModelFile::ModelFile() {
    m_nFileId = -1;
    m_nGroupId = -1;
    m_nVersionId = -1;
    m_nDefineFileId = -1;
    m_nAmountOfChildren = 0;
    m_sTitle = "None";
}

void ModelFile::setId(int nFileId) {
    m_nFileId = nFileId;
}

void ModelFile::setGroupId(int nGroupId) {
    m_nGroupId = nGroupId;
}

void ModelFile::setVersionId(int nVersionId) {
    m_nVersionId = nVersionId;
}

void ModelFile::setDefineFileId(int nDefineFileId) {
    m_nDefineFileId = nDefineFileId;
}

void ModelFile::setAmountOfChildren(int nAmountOfChildren) {
    m_nAmountOfChildren = nAmountOfChildren;
}

void ModelFile::setTitle(const std::string &sTitle) {
    m_sTitle = sTitle;
}

nlohmann::json ModelFile::toJson() const {
    nlohmann::json jsonRet;
    jsonRet["version_id"] = m_nVersionId;
    jsonRet["group_id"] = m_nGroupId;
    jsonRet["def_file_id"] = m_nDefineFileId;
    jsonRet["amount_of_children"] = m_nAmountOfChildren;
    jsonRet["file_id"] = m_nFileId;
    jsonRet["title"] = m_sTitle;
    return jsonRet;
}
