#pragma once

#include <terrarium/core/base.hpp>

#include <mutex>
#include <shared_mutex>
#include <type_traits>
#include <utility>

namespace terra::core {
    template<typename T>
    struct is_lockable : std::bool_constant<requires(T& object) {
            { object.lock() };
            { object.try_lock() };
            { object.unlock() };
        }> {};

    template<typename T>
    constexpr bool is_lockable_v = is_lockable<T>::value;

    template<typename T>
    concept Lockable = is_lockable_v<T>;

    template<typename T>
    struct is_shared_lockable : std::bool_constant<is_lockable_v<T> && requires(T& object) {
            { object.lock_shared() };
            { object.try_lock_shared() };
            { object.unlock_shared() };
        }> {};

    template<typename T>
    constexpr bool is_shared_lockable_v = is_shared_lockable<T>::value;

    template<typename T>
    concept SharedLockable = is_shared_lockable_v<T>;

    template<typename T, Lockable M = std::mutex, Lockable L = std::unique_lock<M>>
        requires std::conjunction_v<
            std::is_constructible<L, M&>,
            std::negation<std::is_reference<T>>
        >
    class LockGuard {
        template<typename T_, Lockable>
            requires std2::is_clean_type_v<T_>
        friend class Mutex;

    public:
        LockGuard(LockGuard&&) noexcept = default;
        LockGuard(const LockGuard&) = delete;

        [[nodiscard]] auto operator*() const noexcept -> T& {
            return *m_object;
        }

        [[nodiscard]] auto operator->() const noexcept -> T* {
            return m_object;
        }

        auto lock() -> void {
            m_lock.lock();
        }

        [[nodiscard]] auto try_lock() -> bool {
            return m_lock.try_lock();
        }

        auto unlock() -> void {
            m_lock.unlock();
        }

    private:
        T* m_object;
        L m_lock;

        explicit LockGuard(T& object, M& mutex)
            : m_object{ &object }, m_lock{ mutex } {}
    };

    template<typename T, SharedLockable M = std::shared_mutex>
    using SharedLockGuard = LockGuard<const T, M, std::shared_lock<M>>;

    template<typename T, Lockable M = std::mutex>
        requires std2::is_clean_type_v<T>
    class Mutex {
    public:
        using Inner = M;

        template<typename... Args>
            requires std::is_constructible_v<T, Args...>
        explicit Mutex(Args&&... args) noexcept(std::is_nothrow_constructible_v<T, Args...>)
            : m_object{ std::forward<Args>(args)... } {}

        Mutex(Mutex&&) noexcept = delete;
        Mutex(const Mutex&) = delete;

        [[nodiscard]] auto lock() -> LockGuard<T, M> {
            return LockGuard<T, M>{ m_object, m_mutex };
        }

        [[nodiscard]] auto shared_lock() const
            requires is_shared_lockable_v<M> {
            return SharedLockGuard<T, M>{ m_object, m_mutex };
        }

    private:
        T m_object;
        mutable M m_mutex{};
    };

    template<typename T, SharedLockable M = std::shared_mutex>
    using SharedMutex = Mutex<T, M>;
}
