#ifndef MODEL_FILE_DIFF_H
#define MODEL_FILE_DIFF_H

#include <string>
#include <json.hpp>

class ModelFileDiff {
    public:
        ModelFileDiff();
        void setId(int nFileId);
        void setParentFileId(int nParentFileId);
        void setVersionId(int nVersionId);
        void setDefineFileId(int nDefineFileId);
        void setAmountOfChildren(int nAmountOfChildren);
        void setGroupId(int nGroupId);
        void setGroupName(const std::string &sGroupName);
        void setFilename(const std::string &sFilename);
        void setFilepath(const std::string &sFilepath);
        void setState(const std::string &sState);
        nlohmann::json toJson() const;

    private:
        std::string TAG;
        int m_nFileId;
        int m_nParentFileId;
        int m_nVersionId;
        int m_nDefineFileId;
        int m_nAmountOfChildren;
        int m_nGroupId;
        std::string m_sGroupName;
        std::string m_sFilename;
        std::string m_sFilepath;
        std::string m_sState;
};

#endif // MODEL_FILE_DIFF_H
