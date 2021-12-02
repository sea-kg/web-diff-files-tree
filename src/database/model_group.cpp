
#include "model_group.h"

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
