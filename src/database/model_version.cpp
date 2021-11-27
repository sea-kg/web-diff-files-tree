
#include "model_version.h"


ModelVersion::ModelVersion() {
    m_nId = -1;
    m_sName = "None";
}

void ModelVersion::setId(int nId) {
    m_nId = nId;
}

void ModelVersion::setName(const std::string &sName) {
    m_sName = sName;
}

nlohmann::json ModelVersion::toJson() {
    nlohmann::json jsonRet;
    jsonRet["version_id"] = m_nId;
    jsonRet["name"] = m_sName;
    return jsonRet;
}
