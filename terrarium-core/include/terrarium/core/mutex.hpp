#pragma once

#include <mutex>
#include <shared_mutex>
#include <type_traits>
#include <utility>

namespace terra::core {
    template<typename T>
    struct is_lockable : std::bool_constant<requires(T& object) {
            { object.lock() };
            { object.unlock() };
        }> {};

    template<typename T>
    constexpr bool is_lockable_v = is_lockable<T>::value;

    template<typename T>
    concept Lockable = is_lockable_v<T>;

    template<typename T>
    struct is_shared_lockable : std::bool_constant<is_lockable_v<T> && requires(T& object) {
            { object.lock_shared() };
            { object.unlock_shared() };
        }> {};

    template<typename T>
    constexpr bool is_shared_lockable_v = is_shared_lockable<T>::value;

    template<typename T>
    concept SharedLockable = is_shared_lockable_v<T>;

    template<typename T, Lockable>
        requires std::negation_v<std::is_const<T>>
    class Mutex;

    template<typename T, Lockable M = std::mutex, template<Lockable> typename L = std::unique_lock>
        requires is_lockable_v<L<M>>
    class LockGuard {
    public:
        explicit LockGuard(Mutex<std::remove_cv_t<T>, M>& mutex)
            : m_object{ &mutex.m_object }, m_lock{ mutex.m_mutex } {}

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

        auto unlock() -> void {
            m_lock.unlock();
        }

    private:
        T* m_object;
        L<M> m_lock;
    };

    template<typename T, SharedLockable M = std::shared_mutex>
    using SharedLockGuard = LockGuard<const T, M, std::shared_lock>;

    template<typename T, Lockable M = std::mutex>
        requires std::negation_v<std::is_const<T>>
    class Mutex {
        template<typename, Lockable M_, template<Lockable> typename L>
            requires is_lockable_v<L<M_>>
        friend class LockGuard;

    public:
        explicit Mutex(auto&&... args) noexcept(std::is_nothrow_constructible_v<T, decltype(args)...>)
            : m_object{ std::forward<decltype(args)>(args)... } {}

        Mutex(Mutex&&) noexcept = delete;
        Mutex(const Mutex&) = delete;

        [[nodiscard]] auto lock() noexcept -> LockGuard<T, M> {
            return LockGuard<T, M>{ *this };
        }

        [[nodiscard]] auto shared_lock() noexcept
            requires is_shared_lockable_v<M> {
            return SharedLockGuard<T, M>{ *this };
        }

    private:
        T m_object;
        M m_mutex{};
    };

    template<typename T, SharedLockable M = std::shared_mutex>
    using SharedMutex = Mutex<T, M>;
}
