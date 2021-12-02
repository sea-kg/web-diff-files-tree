#ifndef MODELS_BASIC_H
#define MODELS_BASIC_H

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

class ModelGroup {
    public:
        ModelGroup();
        void setId(int nId);
        int getId() const;
        void setTitle(const std::string &sTitle);
        const std::string &getTitle() const;
        nlohmann::json toJson() const;

    private:
        std::string TAG;
        int m_nId;
        std::string m_sTitle;
};

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

#endif // MODELS_BASIC_H
