#include <core/debug/assert.hpp>
#include <terrarium/core/debug/assert.hpp>

#include <array>
#include <csignal>

#include <cxxabi.h>
#include <dlfcn.h>
#include <execinfo.h>

namespace terra::core {
    namespace {
        auto crash_handler() noexcept -> void {
            using namespace std::string_view_literals;

            std::array<void*, 64U> frames{};
            const usize count = backtrace(frames.data(), frames.size());

            log_raw("Stacktrace:"sv);
            for(usize i = 2U; i < count; ++i) {
                void* const address = frames[i];
                const auto depth = count - i - 1U;

                Dl_info symbol{};

                std::array<char, 512U> log_buffer{};
                usize log_size = 0U;

                if(dladdr(address, &symbol) && symbol.dli_sname) {
                    const char* mangled_name = symbol.dli_sname;
                    const usize offset = static_cast<byte*>(address) - static_cast<byte*>(symbol.dli_saddr);

                    int status{};
                    char* demangled_name = abi::__cxa_demangle(
                        mangled_name,
                        nullptr,
                        nullptr,
                        &status
                    );

                    const char* name = status == 0 ? demangled_name : mangled_name;

                    log_size = std::format_to_n(
                        log_buffer.begin(),
                        log_buffer.size(),
                        "{:2}: {} ({} +0x{:#})",
                        depth,
                        name,
                        address,
                        offset
                    ).size;

                    std::free(demangled_name);
                } else {
                    log_size = std::format_to_n(
                        log_buffer.begin(),
                        log_buffer.size(),
                        "{:2}: {}",
                        depth,
                        address
                    ).size;
                }

                log_raw(std::string_view{ log_buffer.data(), log_size });
            }
        }

        auto signal_handler(const int signal) noexcept -> void {
            const auto signal_name = [signal] noexcept -> std::string_view {
                using namespace std::string_view_literals;

                switch(signal) {
                case SIGABRT:
                    return "SIGABRT"sv;
                case SIGBUS:
                    return "SIGBUS"sv;
                case SIGFPE:
                    return "SIGFPE"sv;
                case SIGILL:
                    return "SIGILL"sv;
                case SIGSEGV:
                    return "SIGSEGV"sv;
                default:
                    std::unreachable();
                }
            }();

            fatal("The program received a {} signal", signal_name);
            crash_handler();

            struct sigaction sa{};
            sa.sa_flags = SA_SIGINFO;
            sa.sa_handler = SIG_DFL;

            sigemptyset(&sa.sa_mask);

            sigaction(signal, &sa, nullptr);
            std::raise(signal);
        }
    }

    auto register_crash_handler() -> void {
        struct sigaction sa{};
        sa.sa_flags = SA_SIGINFO;
        sa.sa_handler = signal_handler;

        sigemptyset(&sa.sa_mask);

        sigaction(SIGABRT, &sa, nullptr);
        sigaction(SIGBUS, &sa, nullptr);
        sigaction(SIGFPE, &sa, nullptr);
        sigaction(SIGILL, &sa, nullptr);
        sigaction(SIGSEGV, &sa, nullptr);
    }
}
