//
// (c) 2026 Eduardo Doria.
//

#include "LuaBinding.h"

#include "lua.hpp"

#include "LuaBridge.h"
#include "LuaBridgeAddon.h"

#include "input/InputAction.h"
#include "input/InputActionMap.h"
#include "input/InputBinder.h"
#include "Log.h"

using namespace doriax;

void LuaBinding::registerInputClasses(lua_State *L) {
#ifndef DISABLE_LUA_BINDINGS

    luabridge::getGlobalNamespace(L)
        .beginNamespace("doriax")

        .beginNamespace("InputActionType")
            .addVariable("Digital", static_cast<int>(InputActionType::Digital))
            .addVariable("Analog1D", static_cast<int>(InputActionType::Analog1D))
            .addVariable("Analog2D", static_cast<int>(InputActionType::Analog2D))
        .endNamespace()

        .beginNamespace("InputValueType")
            .addVariable("Pressed", static_cast<int>(InputValueType::Pressed))
            .addVariable("Released", static_cast<int>(InputValueType::Released))
            .addVariable("Held", static_cast<int>(InputValueType::Held))
        .endNamespace()

        .beginNamespace("KeyCode")
            .addVariable("A", static_cast<int>(KeyCode::A))
            .addVariable("D", static_cast<int>(KeyCode::D))
            .addVariable("W", static_cast<int>(KeyCode::W))
            .addVariable("S", static_cast<int>(KeyCode::S))
            .addVariable("Space", static_cast<int>(KeyCode::Space))
            .addVariable("Enter", static_cast<int>(KeyCode::Enter))
            .addVariable("Escape", static_cast<int>(KeyCode::Escape))
            .addVariable("Left", static_cast<int>(KeyCode::Left))
            .addVariable("Right", static_cast<int>(KeyCode::Right))
            .addVariable("Up", static_cast<int>(KeyCode::Up))
            .addVariable("Down", static_cast<int>(KeyCode::Down))
            .addVariable("LeftShift", static_cast<int>(KeyCode::LeftShift))
            .addVariable("LeftControl", static_cast<int>(KeyCode::LeftControl))
        .endNamespace()

        .beginNamespace("GamepadButton")
            .addVariable("A", static_cast<int>(GamepadButton::A))
            .addVariable("B", static_cast<int>(GamepadButton::B))
            .addVariable("X", static_cast<int>(GamepadButton::X))
            .addVariable("Y", static_cast<int>(GamepadButton::Y))
            .addVariable("LB", static_cast<int>(GamepadButton::LB))
            .addVariable("RB", static_cast<int>(GamepadButton::RB))
            .addVariable("Start", static_cast<int>(GamepadButton::Start))
        .endNamespace()

        .beginNamespace("GamepadAxis")
            .addVariable("LeftStickX", static_cast<int>(GamepadAxis::LeftStickX))
            .addVariable("LeftStickY", static_cast<int>(GamepadAxis::LeftStickY))
            .addVariable("RightStickX", static_cast<int>(GamepadAxis::RightStickX))
            .addVariable("RightStickY", static_cast<int>(GamepadAxis::RightStickY))
            .addVariable("LeftTrigger", static_cast<int>(GamepadAxis::LeftTrigger))
            .addVariable("RightTrigger", static_cast<int>(GamepadAxis::RightTrigger))
        .endNamespace()

        .beginClass<InputAction>("InputAction")
            .addFunction("name", &InputAction::name)
            .addFunction("setName", &InputAction::setName)
            .addFunction("isPressed", &InputAction::isPressed)
            .addFunction("isHeld", &InputAction::isHeld)
            .addFunction("getValue", &InputAction::getValue)
            .addFunction("getX", &InputAction::getX)
            .addFunction("getY", &InputAction::getY)
            .addFunction("addCallback", +[](InputAction* self, const luabridge::LuaRef& func) -> int {
                if (!func.isFunction()) {
                    Log::error("InputAction.addCallback: argument must be a function");
                    return -1;
                }
                return self->addCallback([func](const InputValue& val) {
                    try {
                        func(val.value, val.x, val.y);
                    } catch (const std::exception& e) {
                        Log::error("InputAction callback error: %s", e.what());
                    }
                });
            })
        .endClass()

        .beginClass<InputActionMap>("InputActionMap")
            .addFunction("name", &InputActionMap::name)
            .addFunction("addAction", +[](InputActionMap* self, const std::string& name, InputActionType type) {
                return self->addAction(name, type);
            })
            .addFunction("getAction", &InputActionMap::getAction)
            .addFunction("hasAction", &InputActionMap::hasAction)
            .addFunction("enable", &InputActionMap::enable)
            .addFunction("disable", &InputActionMap::disable)
            .addFunction("isEnabled", &InputActionMap::isEnabled)
            .addFunction("listActions", &InputActionMap::listActions)
        .endClass()

        .beginNamespace("InputActionMapRegistry")
            .addFunction("get", +[]() {
                return &InputActionMapRegistry::get();
            })
            .addFunction("createMap", +[](const std::string& name) {
                return InputActionMapRegistry::get().createMap(name);
            })
            .addFunction("getMap", +[](const std::string& name) {
                return InputActionMapRegistry::get().getMap(name);
            })
            .addFunction("activateMap", +[](const std::string& name) {
                InputActionMapRegistry::get().activateMap(name);
            })
            .addFunction("getActiveMap", +[]() {
                return InputActionMapRegistry::get().getActiveMap();
            })
            .addFunction("listMaps", +[]() {
                return InputActionMapRegistry::get().listMaps();
            })
        .endNamespace()

        .beginNamespace("InputBinder")
            .addFunction("get", +[]() {
                return &InputBinder::get();
            })
            .addFunction("bindKey", +[](KeyCode key, const std::string& action, float scale) {
                InputBinder::get().bindKey(key, action, scale);
            })
            .addFunction("bindGamepadButton", +[](GamepadButton button, const std::string& action) {
                InputBinder::get().bindGamepadButton(button, action);
            })
            .addFunction("bindGamepadAxis", +[](GamepadAxis axis, const std::string& action, float scale, float deadzone) {
                InputBinder::get().bindGamepadAxis(axis, action, scale, deadzone);
            })
            .addFunction("remapBinding", +[](const std::string& action, KeyCode oldKey, KeyCode newKey) {
                InputBinder::get().remapBinding(action, oldKey, newKey);
            })
        .endNamespace()

        .endNamespace();

    Log::info("Input action system Lua bindings registered");

#endif // DISABLE_LUA_BINDINGS
}
