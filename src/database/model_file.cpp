
#include "model_file.h"

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
