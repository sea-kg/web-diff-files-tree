#ifndef MODELS_CACHE_H
#define MODELS_CACHE_H

#include <string>
#include <map>
#include <unordered_map>

struct ModelCacheFiles {
    int findFileId(int nDefineFileId);
    void add(int nDefineFileId, int nFileId);
    std::unordered_map<int, int> m_mapCacheFiles;
};

struct ModelCacheGroups {
    int findFileId(int nGroupId, int nDefineFileId);
    void add(int nGroupId, int nDefineFileId, int nFileId);
    std::unordered_map<int, ModelCacheFiles> m_mapCacheFileDefinesIds;
};


struct ModelCacheFilesVersion {
    int findFileId(int nVersionId, int nGroupId, int nDefineFileId);
    void add(int nVersionId, int nGroupId, int nDefineFileId, int nFileId);
    std::unordered_map<int, ModelCacheGroups> m_mapCacheGroups;
};

#endif // MODELS_CACHE_H
