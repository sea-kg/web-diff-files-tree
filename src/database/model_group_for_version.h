#ifndef MODEL_GROUP_FOR_VERSION_H
#define MODEL_GROUP_FOR_VERSION_H

#include <string>
#include <json.hpp>

class ModelGroupForVersion {
    public:
        ModelGroupForVersion();
        void setId(int nId);
        void setVersionId(int nId);
        void setTitle(const std::string &sTitle);
        void setAmountOfFiles(int nId);

        nlohmann::json toJson() const;

    private:
        std::string TAG;
        int m_nGroupId;
        int m_nVersionId;
        std::string m_sTitle;
        int m_nAmountOfFiles;
};

#endif // MODEL_GROUP_FOR_VERSION_H
