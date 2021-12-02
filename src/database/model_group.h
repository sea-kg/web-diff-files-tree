#ifndef MODEL_GROUP_H
#define MODEL_GROUP_H

#include <string>
#include <json.hpp>

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

#endif // MODEL_GROUP_H
