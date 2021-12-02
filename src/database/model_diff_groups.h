#ifndef MODEL_DIFF_GROUPS_H
#define MODEL_DIFF_GROUPS_H

#include <string>
#include <json.hpp>
#include <algorithm>
#include "model_group.h"

class ModelDiffFile {
    public:
        ModelDiffFile();
        void setId(int nFileId);
        int getId() const;
        void setParentFileId(int nParentFileId);
        int getParentFileId() const;
        void setVersionId(int nVersionId);
        void setDefineFileId(int nDefineFileId);
        void setAmountOfChildren(int nAmountOfChildren);
        void setFilename(const std::string &sFilename);
        void setFilepath(const std::string &sFilepath);
        void setState(const std::string &sState);
        const std::string &getState() const;
        nlohmann::json toJson() const;

    private:
        std::string TAG;
        int m_nFileId;
        int m_nParentFileId;
        int m_nVersionId;
        int m_nDefineFileId;
        int m_nAmountOfChildren;
        std::string m_sFilename;
        std::string m_sFilepath;
        std::string m_sState;
};

class ModelDiffGroup {
    public:
        ModelDiffGroup();
        ModelDiffGroup(const ModelGroup &group);
        void addDiffFile(const ModelDiffFile &fileDiff);
        const ModelGroup &getGroup() const;
        const std::vector<int> &getFilesIds() const;
        const std::vector<int> &getParentFilesIds() const;
        bool hasFileId(int nFileId) const;
        nlohmann::json toJson() const;

    private:
        std::string TAG;
        ModelGroup m_group;
        int m_nNewFiles;
        int m_nMissingFiles;
        std::vector<ModelDiffFile> m_vFileDiffs;
        std::vector<int> m_vFilesIds;
        std::vector<int> m_vParentFilesIds;
};

class ModelDiffGroups {
    public:
        ModelDiffGroups();
        void addDiffFile(
            const ModelGroup &group,
            const ModelDiffFile &fileDiff
        );
        const std::map<int, ModelDiffGroup> &getGroups();
        nlohmann::json toJson() const;

    private:
        std::string TAG;
        std::map<int, ModelDiffGroup> m_mapGroups;
        // std::map<int, ModelComments> m_mapComments;
};

#endif // MODEL_DIFF_GROUPS_H
