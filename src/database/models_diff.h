#ifndef MODELS_DIFF_H
#define MODELS_DIFF_H

#include <string>
#include <json.hpp>
#include <algorithm>
#include "models_basic.h"

class ModelDiffFile {
    public:
        ModelDiffFile();
        void setId(int nFileId);
        int getId() const;
        void setParentFileId(int nParentFileId);
        int getParentFileId() const;
        void setVersionId(int nVersionId);
        void setDefineFileId(int nDefineFileId);
        int getDefineFileId() const;
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
        int size() const;
        void addDiffFile(const ModelDiffFile &fileDiff);
        const ModelGroup &getGroup() const;
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

class ModelComment {
    public:
        ModelComment();
        void setId(int nId);
        void setDefineFileId(int nId);
        void setComment(const std::string &sTitle);

        nlohmann::json toJson() const;

    private:
        std::string TAG;
        int m_nCommentId;
        int m_nDefineFileId;
        std::string m_sComment;
};

class ModelDiffGroups {
    public:
        ModelDiffGroups();
        void addDiffFile(
            const ModelGroup &group,
            const ModelDiffFile &fileDiff
        );
        int getFilesLength() const;
        int getCommentsLength() const;
        const std::map<int, ModelDiffGroup> &getGroups();
        const std::vector<int> &getFilesDefinesIds() const;
        void setComments(int nDefineFileId, std::vector<ModelComment> vComments);
        nlohmann::json toJson() const;

    private:
        std::string TAG;
        std::map<int, ModelDiffGroup> m_mapGroups;
        std::vector<int> m_vFileDefineIds;
        std::map<int, std::vector<ModelComment>> m_mapComments;
};

#endif // MODELS_DIFF_H
