#pragma once

#include <vector>
#include <memory>

#include "tctdisplay.hpp"
#include "tctinput.hpp"
#include "tctfiles.hpp"

namespace tct {

struct SceneContext {};

/// @brief A self-contained instance that gets switched in and out of.
class Scene {
   private:
    std::unique_ptr<SceneContext> context;
    std::vector<Asset> assets;

   public:
    Scene() {};
    virtual void onInit() = 0;
    virtual void onInput(const InputKey key) = 0;
    virtual void onUpdate() = 0;
    virtual std::vector<Renderable> onRender() = 0;
    virtual void onDestroy() = 0;
    virtual ~Scene() = 0;
};

} // namespace tct