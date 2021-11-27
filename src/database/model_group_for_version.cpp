
#include "model_group_for_version.h"

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
