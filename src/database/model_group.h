#ifndef MODEL_GROUP_H
#define MODEL_GROUP_H

#include <string>
#include <json.hpp>

class ModelGroup {
    public:
        ModelGroup();
        void setId(int nId);
        void setName(const std::string &sName);
        nlohmann::json toJson() const;

    private:
        std::string TAG;
        int m_nId;
        std::string m_sName;
};

#endif // MODEL_GROUP_H
