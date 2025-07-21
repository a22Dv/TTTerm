#pragma once

#include <memory>
#include <vector>

#include "tctobject.hpp"

namespace tct {

struct SceneContext {};
class Scene {
   private:
    std::unique_ptr<SceneContext> context{};
    std::vector<std::unique_ptr<Object>> objects{};
    bool active{};

   public:
    void setActiveStatus(const bool nVal) { active = nVal; };
    bool getActiveStatus() { return active; };
    virtual void init() = 0;
    virtual void onStart();
    virtual void onUpdate() = 0;
    virtual void onDestroy();
};

} // namespace tct