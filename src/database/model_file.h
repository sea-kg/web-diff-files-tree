#ifndef MODEL_FILE_H
#define MODEL_FILE_H

#include <string>
#include <json.hpp>

class ModelFile {
    public:
        ModelFile();
        void setId(int nFileId);
        void setGroupId(int nGroupId);
        void setVersionId(int nVersionId);
        void setDefineFileId(int nDefineFileId);
        void setAmountOfChildren(int nAmountOfChildren);
        void setTitle(const std::string &sTitle);

        nlohmann::json toJson() const;

    private:
        std::string TAG;
        int m_nFileId;
        int m_nGroupId;
        int m_nVersionId;
        int m_nDefineFileId;
        int m_nAmountOfChildren;
        std::string m_sTitle;
};

#endif // MODEL_FILE_H
