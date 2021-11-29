#ifndef MODEL_VERSION_H
#define MODEL_VERSION_H

#include <string>
#include <json.hpp>

class ModelVersion {
    public:
        ModelVersion();
        void setId(int nId);
        int getId();
        void setName(const std::string &sName);
        nlohmann::json toJson() const;

    private:
        std::string TAG;
        int m_nId;
        std::string m_sName;
};

#endif // MODEL_VERSION_H
