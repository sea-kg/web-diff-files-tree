
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

const std::string &ModelVersion::getName() {
    return m_sName;
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

int ModelFile::getId() {
    return m_nFileId;
}

void ModelFile::setGroupId(int nGroupId) {
    m_nGroupId = nGroupId;
}

int ModelFile::getGroupId() {
    return m_nGroupId;
}

void ModelFile::setVersionId(int nVersionId) {
    m_nVersionId = nVersionId;
}

int ModelFile::getVersionId() {
    return m_nVersionId;
}

void ModelFile::setDefineFileId(int nDefineFileId) {
    m_nDefineFileId = nDefineFileId;
}

int ModelFile::getDefineFileId() {
    return m_nDefineFileId;
}

void ModelFile::setAmountOfChildren(int nAmountOfChildren) {
    m_nAmountOfChildren = nAmountOfChildren;
}

int ModelFile::getAmountOfChildren() {
    return m_nAmountOfChildren;
}

void ModelFile::setTitle(const std::string &sTitle) {
    m_sTitle = sTitle;
}

void ModelFile::setPath(const std::string &sPath) {
    m_sPath = sPath;
}

void ModelFile::setSize(int nSize) {
    m_nSize = nSize;
}

void ModelFile::setCompressSize(int nCompressSize) {
    m_nCompressSize = nCompressSize;
}

void ModelFile::setMode(const std::string &sMode) {
    m_sMode = sMode;
}

void ModelFile::setIsDir(bool bIsDir) {
    m_bIsDir = bIsDir;
}

void ModelFile::setDatetime(const std::string &sDatetime) {
    m_sDatetime = sDatetime;
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
