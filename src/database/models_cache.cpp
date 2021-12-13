
#include "models_cache.h"

int ModelCacheFiles::findFileId(int nDefineFileId) {
    int nRet = -1;
    std::unordered_map<int, int>::iterator it;
    it = m_mapCacheFiles.find(nDefineFileId);
    if (it != m_mapCacheFiles.end()) {
        nRet = it->second;
    }
    return nRet;
}

void ModelCacheFiles::add(int nDefineFileId, int nFileId) {
    m_mapCacheFiles[nDefineFileId] = nFileId;
}

int ModelCacheGroups::findFileId(int nGroupId, int nDefineFileId) {
    int nRet = -1;
    std::unordered_map<int, ModelCacheFiles>::iterator it;
    it = m_mapCacheFileDefinesIds.find(nGroupId);
    if (it != m_mapCacheFileDefinesIds.end()) {
        nRet = it->second.findFileId(nDefineFileId);
    }
    return nRet;
}

void ModelCacheGroups::add(int nGroupId, int nDefineFileId, int nFileId) {
    std::unordered_map<int, ModelCacheFiles>::iterator it;
    it = m_mapCacheFileDefinesIds.find(nGroupId);
    if (it == m_mapCacheFileDefinesIds.end()) {
        m_mapCacheFileDefinesIds[nGroupId] = ModelCacheFiles();
    }
    m_mapCacheFileDefinesIds[nGroupId].add(nDefineFileId, nFileId);
}

int ModelCacheFilesVersion::findFileId(int nVersionId, int nGroupId, int nDefineFileId) {
    int nRet = -1;
    std::unordered_map<int, ModelCacheGroups>::iterator it;
    it = m_mapCacheGroups.find(nVersionId);
    if (it != m_mapCacheGroups.end()) {
        nRet = it->second.findFileId(nGroupId, nDefineFileId);
    }
    return nRet;
}

void ModelCacheFilesVersion::add(int nVersionId, int nGroupId, int nDefineFileId, int nFileId) {
    std::unordered_map<int, ModelCacheGroups>::iterator it;
    it = m_mapCacheGroups.find(nVersionId);
    if (it == m_mapCacheGroups.end()) {
        m_mapCacheGroups[nVersionId] = ModelCacheGroups();
    }
    m_mapCacheGroups[nVersionId].add(nGroupId, nDefineFileId, nFileId);
}