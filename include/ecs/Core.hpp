//
// Created by benito on 1/8/18.
//

#ifndef RTYPE_CORE_HPP
#define RTYPE_CORE_HPP

/* Core class of the engine */
#include <memory>
#include <DLLoader/DLLoader.hpp>
#include "Mediator.hpp"
#include "Module.hpp"

namespace Af {
    struct Event
    {
        enum class Type
        {
            UNDEFINED,
            END_LOOP
        };

        Event(Type type) : t(type) {}

        Type t;
    };
    class Core : public Module<Core, Event> {
    public:
        Core(/*const std::string& entityDir, const std::string &systemDir*/);
        ~Core() = default;

        Core(const Core&) = delete;
        Core(Core&&) = default;


        template<typename ST, typename... Args>
        void emplaceSystem(Args&&... args) {
            static_assert(std::is_base_of<ASystem, ST>::value,
                          "addSystem function should be called with a type inheriting from ASystem");
            static_assert(utils::is_named<ST>::value,
                            "You should have a classname that rocks. Refer to Named class for details.");

            auto s = std::make_unique<ST>(mediator, std::forward(args...));
            mediator->addSystem(s.get());
            _systems.emplace(utils::IndexType::get<ST>(), std::move(s));
        }

        template <typename... Systems>
        void loadSystems()
        {
            (emplaceSystem<Systems>(),...);
        }

        template <typename ST>
        ST& getSystem()
        {
            auto& ptr= _systems.at(utils::IndexType::get<ST>());
            return (static_cast<ST&>(*ptr));
        }

        template <typename ST>
        const ST& getSystem() const
        {
            auto& ptr= _systems.at(utils::IndexType::get<ST>());
            return (static_cast<ST&>(*ptr));
        }

        void loadSystemsIn(const std::string &dirName);
        void loadEntitiesIn(const std::string &dirName);

        void go();
        utils::DLLoader& getDLLoader();
        void handle(const Event&);

        void setEntityDir(const std::string &);
        void setSystemDir(const std::string &);

    private:
        bool isOver;
        std::unordered_map<utils::IndexType::meta, std::unique_ptr<ASystem>> _systems;

        std::string entitiesDir;
        std::string systemsDir;
        utils::DLLoader dlLoader;
    };
}

#endif //RTYPE_CORE_HPP
