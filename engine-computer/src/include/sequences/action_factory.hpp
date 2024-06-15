#include <string>
#include <memory>
#include <unordered_map>
#include <functional>
#include "sequences/action.hpp"

namespace mach {

/**
 * @brief Factory class for creating actions.
 */
class ActionFactory {
    public:
        static ActionFactory& getInstance() {
            static ActionFactory instance;
            return instance;
        }

        ActionFactory(ActionFactory const&) = delete;
        void operator=(ActionFactory const&) = delete;

        template <typename T>
        void registerAction(const std::string& action);
    
    private:
        ActionFactory() {}

        std::unique_ptr<Action> createAction(const std::string& action);
    };
}