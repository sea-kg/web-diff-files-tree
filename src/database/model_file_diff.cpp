
#include "model_file_diff.h"

ModelFileDiff::ModelFileDiff() {
    m_nFileId = -1;
    m_nParentFileId = -1;
    m_nVersionId = -1;
    m_nDefineFileId = -1;
    m_nAmountOfChildren = 0;
    m_sFilename = "None";
    m_sFilepath = "None";
    m_nGroupId = -1;
    m_sGroupName = "None";
    m_sState = "None";
}

void ModelFileDiff::setId(int nFileId) {
    m_nFileId = nFileId;
}

void ModelFileDiff::setParentFileId(int nParentFileId) {
    m_nParentFileId = nParentFileId;
}

void ModelFileDiff::setVersionId(int nVersionId) {
    m_nVersionId = nVersionId;
}

void ModelFileDiff::setDefineFileId(int nDefineFileId) {
    m_nDefineFileId = nDefineFileId;
}

void ModelFileDiff::setAmountOfChildren(int nAmountOfChildren) {
    m_nAmountOfChildren = nAmountOfChildren;
}

void ModelFileDiff::setGroupId(int nGroupId) {
    m_nGroupId = nGroupId;
}

void ModelFileDiff::setGroupName(const std::string &sGroupName) {
    m_sGroupName = sGroupName;
}

void ModelFileDiff::setFilename(const std::string &sFilename) {
    m_sFilename = sFilename;
}

void ModelFileDiff::setFilepath(const std::string &sFilepath) {
    m_sFilepath = sFilepath;
}

void ModelFileDiff::setState(const std::string &sState) {
    m_sState = sState;
}

nlohmann::json ModelFileDiff::toJson() const {
    nlohmann::json jsonRet;
    jsonRet["id"] = "id" + std::to_string(m_nFileId);
    jsonRet["parent"] = "id" + std::to_string(m_nParentFileId);
    jsonRet["group_name"] = m_sGroupName;
    jsonRet["define_file_id"] = m_nDefineFileId;
    jsonRet["filename"] = m_sFilename;
    jsonRet["amount_of_children"] = m_nAmountOfChildren;
    jsonRet["state"] = m_sState;
    // jsonRet["comments"] => load_comments($conn, intval($define_file_id)),
    return jsonRet;
}
