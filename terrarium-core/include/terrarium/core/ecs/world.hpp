#pragma once

#include <terrarium/core/base.hpp>
#include <terrarium/core/mutex.hpp>
#include <terrarium/core/debug/assert.hpp>
#include <terrarium/core/ecs/extractor.hpp>
#include <terrarium/core/ecs/resource.hpp>
#include <terrarium/core/utils/locator.hpp>

#include <functional>
#include <queue>
#include <type_traits>
#include <utility>

namespace terra::core {
    class App;

    class TERRA_CORE_API World {
        friend class Commands;

        class CommandQueue {
            using Command = std::move_only_function<void(World&)>;

        public:
            explicit CommandQueue() noexcept = default;

            CommandQueue(CommandQueue&&) noexcept = delete;
            CommandQueue(const CommandQueue&) = delete;

            template<Resource R, typename... Args>
                requires std::conjunction_v<
                    std::is_constructible<R, Args...>,
                    std2::is_clean_type<std::remove_reference_t<R>>
                >
            auto make_resource(Args&&... args) -> void;

            auto flush(World& world) -> void;

        private:
            Mutex<std::queue<Command>> m_commands{};
        };

    public:
        World() = default;
        World(World&&) noexcept = delete;
        World(const World&) = delete;

        template<Resource R, typename... Args>
            requires std::conjunction_v<
                std::is_constructible<R, Args...>,
                std2::is_clean_type<std::remove_reference_t<R>>
            >
        auto make_resource(Args&&... args) -> void {
            m_resources.make_object<R>(std::forward<Args>(args)...);
        }

        template<Resource R>
            requires std2::is_clean_type_v<std::remove_reference_t<R>>
        [[nodiscard]] decltype(auto) get_resource(this auto&& self) noexcept {
            auto* resource = self.m_resources.template get_object<R>();
            TERRA_DEBUG_ASSERT(resource != nullptr, "Resource '{}' not found", typeid(R).name());

            return *resource;
        }

        auto flush() -> void;

    private:
        CommandQueue m_queue;

        Locator m_resources{};
    };

    template<Resource R, typename... Args>
        requires std::conjunction_v<
            std::is_constructible<R, Args...>,
            std2::is_clean_type<std::remove_reference_t<R>>
        >
    auto World::CommandQueue::make_resource(Args&&... args) -> void {
        auto commands = m_commands.lock();
        commands->emplace(
            [...args = std::forward<Args>(args)](World& world) mutable -> void {
                world.make_resource<R>(std::move(args)...);
            }
        );
    }

    class TERRA_CORE_API Commands {
    public:
        explicit Commands(World& world) noexcept;

        template<Resource R, typename... Args>
            requires std::conjunction_v<
                std::is_constructible<R, Args...>,
                std2::is_clean_type<std::remove_reference_t<R>>
            >
        auto make_resource(Args&&... args) -> void {
            m_queue.make_resource<R>(std::forward<Args>(args)...);
        }

    private:
        World::CommandQueue& m_queue;
    };

    template<>
    struct TERRA_CORE_API IExtractor<Commands> {
        [[nodiscard]] static auto operator()(World& world, App&) noexcept -> Commands;
    };

    template<typename>
    class Res;

    template<Resource R>
    class Res<R> {
        using ReferenceType = std::add_lvalue_reference_t<R>;
        using PointerType = std::add_pointer_t<R>;
        using WorldType = std::conditional_t<std::is_const_v<std::remove_reference_t<R>>, const World, World>;

    public:
        explicit Res(WorldType& world) noexcept
            : m_object{ static_cast<ReferenceType>(world.template get_resource<std2::remove_ref_cv_t<R>>()) } {}

        [[nodiscard]] auto operator*() const noexcept -> ReferenceType {
            return m_object;
        }

        [[nodiscard]] auto operator->() const noexcept -> PointerType {
            return &m_object;
        }

    protected:
        ReferenceType& m_object;
    };

    template<Resource R>
    struct IExtractor<Res<R>> {
        [[nodiscard]] static auto operator()(World& world, App&) -> Res<R> {
            return Res<R>{ world };
        }
    };

    template<Resource R>
    struct IExtractor<Res<const R>> {
        [[nodiscard]] static auto operator()(const World& world, const App&) -> Res<const R> {
            return Res<const R>{ world };
        }
    };
}
