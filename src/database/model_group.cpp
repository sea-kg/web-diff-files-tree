
#include "model_group.h"

ModelGroup::ModelGroup() {
    m_nId = -1;
    m_sName = "None";
}

void ModelGroup::setId(int nId) {
    m_nId = nId;
}

void ModelGroup::setName(const std::string &sName) {
    m_sName = sName;
}

nlohmann::json ModelGroup::toJson() const {
    nlohmann::json jsonRet;
    jsonRet["group_id"] = m_nId;
    jsonRet["name"] = m_sName;
    return jsonRet;
}
