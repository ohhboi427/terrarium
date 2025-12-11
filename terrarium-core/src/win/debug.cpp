#include <debug.hpp>
#include <terrarium/debug.hpp>

#include <array>
#include <bit>
#include <csignal>
#include <new>

#include <Windows.h>
#include <DbgHelp.h>

#pragma comment(lib, "DbgHelp.lib")

namespace terra {
    namespace {
        auto crash_handler() noexcept -> void {
            using namespace std::string_view_literals;

            const HANDLE process = GetCurrentProcess();

            std::array<PVOID, 64U> frames{};
            const usize count = CaptureStackBackTrace(2U, frames.size(), frames.data(), nullptr);

            SymInitialize(process, nullptr, TRUE);

            log_raw("Stacktrace:"sv);
            for(usize i = 0U; i != count; ++i) {
                const PVOID address = frames[i];
                const auto depth = count - i - 1U;

                std::array<byte, sizeof(SYMBOL_INFO) + 256U> symbol_buffer{};
                SYMBOL_INFO& symbol = *std::launder(reinterpret_cast<SYMBOL_INFO*>(symbol_buffer.data()));
                symbol.SizeOfStruct = sizeof(SYMBOL_INFO);
                symbol.MaxNameLen = symbol_buffer.size() - sizeof(SYMBOL_INFO);

                std::array<char, 512U> log_buffer{};
                usize log_size = 0U;

                usize offset{};
                if(SymFromAddr(process, std::bit_cast<DWORD64>(address), &offset, &symbol)) {
                    log_size = std::format_to_n(
                        log_buffer.begin(),
                        log_buffer.size(),
                        "{:2}: {} ({} +0x{:#})",
                        depth,
                        symbol.Name,
                        address,
                        offset
                    ).size;
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

            SymCleanup(process);
        }

        auto signal_handler(const int signal) noexcept -> void {
            const auto signal_name = [signal] noexcept -> std::string_view {
                using namespace std::string_view_literals;

                switch(signal) /* NOLINT */ {
                case SIGABRT:
                    return "SIGABRT"sv;
                default:
                    std::unreachable();
                }
            }();

            fatal("The program received a {} signal", signal_name);
            crash_handler();

            std::signal(signal, SIG_DFL);
            std::raise(signal);
        }

        auto WINAPI unhandled_exception_handler(EXCEPTION_POINTERS* exception) noexcept -> LONG {
            const auto exception_name = [exception] noexcept -> std::string_view {
                using namespace std::string_view_literals;

                switch(exception->ExceptionRecord->ExceptionCode) {
                case EXCEPTION_ACCESS_VIOLATION:
                    return "EXCEPTION_ACCESS_VIOLATION"sv;
                case EXCEPTION_ARRAY_BOUNDS_EXCEEDED:
                    return "EXCEPTION_ARRAY_BOUNDS_EXCEEDED"sv;
                case EXCEPTION_BREAKPOINT:
                    return "EXCEPTION_BREAKPOINT"sv;
                case EXCEPTION_DATATYPE_MISALIGNMENT:
                    return "EXCEPTION_DATATYPE_MISALIGNMENT"sv;
                case EXCEPTION_FLT_DENORMAL_OPERAND:
                    return "EXCEPTION_FLT_DENORMAL_OPERAND"sv;
                case EXCEPTION_FLT_DIVIDE_BY_ZERO:
                    return "EXCEPTION_FLT_DIVIDE_BY_ZERO"sv;
                case EXCEPTION_FLT_INEXACT_RESULT:
                    return "EXCEPTION_FLT_INEXACT_RESULT"sv;
                case EXCEPTION_FLT_INVALID_OPERATION:
                    return "EXCEPTION_FLT_INVALID_OPERATION"sv;
                case EXCEPTION_FLT_OVERFLOW:
                    return "EXCEPTION_FLT_OVERFLOW"sv;
                case EXCEPTION_FLT_STACK_CHECK:
                    return "EXCEPTION_FLT_STACK_CHECK"sv;
                case EXCEPTION_FLT_UNDERFLOW:
                    return "EXCEPTION_FLT_UNDERFLOW"sv;
                case EXCEPTION_ILLEGAL_INSTRUCTION:
                    return "EXCEPTION_ILLEGAL_INSTRUCTION"sv;
                case EXCEPTION_IN_PAGE_ERROR:
                    return "EXCEPTION_IN_PAGE_ERROR"sv;
                case EXCEPTION_INT_DIVIDE_BY_ZERO:
                    return "EXCEPTION_INT_DIVIDE_BY_ZERO"sv;
                case EXCEPTION_INT_OVERFLOW:
                    return "EXCEPTION_INT_OVERFLOW"sv;
                case EXCEPTION_INVALID_DISPOSITION:
                    return "EXCEPTION_INVALID_DISPOSITION"sv;
                case EXCEPTION_NONCONTINUABLE_EXCEPTION:
                    return "EXCEPTION_NONCONTINUABLE_EXCEPTION"sv;
                case EXCEPTION_PRIV_INSTRUCTION:
                    return "EXCEPTION_PRIV_INSTRUCTION"sv;
                case EXCEPTION_SINGLE_STEP:
                    return "EXCEPTION_SINGLE_STEP"sv;
                case EXCEPTION_STACK_OVERFLOW:
                    return "EXCEPTION_STACK_OVERFLOW"sv;
                default:
                    std::unreachable();
                }
            }();

            fatal("An unhandled {} exception happened", exception_name);
            crash_handler();

            return EXCEPTION_EXECUTE_HANDLER;
        }
    }

    auto register_crash_handler() -> void {
        std::signal(SIGABRT, signal_handler);

        SetUnhandledExceptionFilter(unhandled_exception_handler);
    }
}
