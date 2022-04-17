/// Error handling utility.
///
/// 'Result<T, E>' type is used for returning and handling errors.
/// It is a tagged union that has 2 possible variants:
/// * 'Ok<T>' - representing success and containing a value
/// * 'Err<E>' - representing failure and containing an error value
///
/// The state of a result can be checked with 'is_ok' and 'is_err' methods.
///
/// There are multiple methods that extract the value contained in a
/// Result<T, E>. If the Result is Err then:
/// * 'unwrap' - panics with generic message,
/// * 'expect' - panics with provided message,
/// * 'unwrap_or_else' - returns result of executing provided function.
///
/// 'unwrap_err' may be used to extract contained error value.
///
/// 'Unit' type can be used in place of T for functions that may fail
///  but do not return a value.
///
/// # Examples
///
/// ```
/// Result<int, int> make_result() {
///     if (failure) { return Err{-1}; }
///     return Ok{1};
/// }
/// ```
///
/// ```
/// Result<File, ErrorKind> open_file(std::string_view path);
/// Result<File, ErrorKind> create_file(std::string_view path);
///
/// File file = open_file(path).unwrap_or_else([&](ErrorKind kind) {
///     switch (kind) {
///         case ErrorKind::NotFound:
///             return create_file(path).expect("Cannot create file");
///         case ErrorKind::PermissionDenied:
///             panic("Permission denied");
///     }
/// });
/// ```

#ifndef CCL_RESULT_HPP
#define CCL_RESULT_HPP

#include <utility>

#include "branch_prediction.hpp"
#include "panic.hpp"

namespace ccl {

struct Unit {};

template <typename T>
class Ok {
    T value_;

   public:
    constexpr explicit Ok(const T& t) : value_ { t } {}

    constexpr explicit Ok(T&& t) : value_ { std::move(t) } {}

    constexpr T& value() & {
        return value_;
    }

    constexpr const T& value() const& {
        return value_;
    }

    constexpr T&& value() && {
        return std::move(value_);
    }

    constexpr const T&& value() const&& {
        return std::move(value_);
    }
};

template <typename E>
class Err {
    E value_;

   public:
    constexpr explicit Err(const E& e) : value_ { e } {}

    constexpr explicit Err(E&& e) : value_ { std::move(e) } {}

    constexpr E& value() & {
        return value_;
    }

    constexpr const E& value() const& {
        return value_;
    }

    constexpr E&& value() && {
        return std::move(value_);
    }

    constexpr const E&& value() const&& {
        return std::move(value_);
    }
};

namespace detail {

// clang-format off

template <typename T>
inline constexpr bool has_trivial_copy_move_destruct =
    std::is_trivially_copy_constructible_v<T>
    && std::is_trivially_copy_assignable_v<T>
    && std::is_trivially_move_constructible_v<T>
    && std::is_trivially_move_assignable_v<T>
    && std::is_trivially_destructible_v<T>;

// clang-format on

template <
    typename T,
    typename E,
    bool =
        has_trivial_copy_move_destruct<T>&& has_trivial_copy_move_destruct<E>>
struct ResultBase {
    union {
        T ok;
        E err;
    };

    bool is_ok_;

    // NOLINTNEXTLINE(hicpp-explicit-conversions)
    ResultBase(Ok<T>&& ok) : ok { std::move(ok).value() }, is_ok_ { true } {}

    // NOLINTNEXTLINE(hicpp-explicit-conversions)
    ResultBase(Err<E>&& err) :
        err { std::move(err).value() },
        is_ok_ { false } {}
};

template <typename T, typename E>
struct ResultBase<T, E, false> {
    union {
        T ok;
        E err;
    };

    bool is_ok_;

    // NOLINTNEXTLINE(hicpp-explicit-conversions)
    ResultBase(Ok<T>&& ok) : ok { std::move(ok).value() }, is_ok_ { true } {}

    // NOLINTNEXTLINE(*-explicit-conversions)
    ResultBase(Err<E>&& err) :
        err { std::move(err).value() },
        is_ok_ { false } {}

    // NOLINTNEXTLINE(*-member-init)
    ResultBase(const ResultBase& other) {
        construct(other);
    }

    // NOLINTNEXTLINE(*-member-init)
    ResultBase(ResultBase&& other) noexcept {
        construct(std::move(other));
    }

    // NOLINTNEXTLINE(cert-oop54-cpp)
    ResultBase& operator=(const ResultBase& other) {
        assign(other);
        return *this;
    }

    ResultBase& operator=(ResultBase&& other) noexcept {
        assign(std::move(other));
        return *this;
    }

    ~ResultBase() {
        if (is_ok_) {
            if constexpr (!std::is_trivially_destructible_v<T>) {
                ok.~T();  // NOLINT (*-union-access)
            }
        } else {
            if constexpr (!std::is_trivially_destructible_v<E>) {
                err.~E();  // NOLINT (*-union-access)
            }
        }
    };

    template <typename R>
    void construct(R&& other) {
        this->is_ok_ = other.is_ok_;
        if (this->is_ok_) {
            new (&this->ok) auto(std::forward<R>(other).ok);
        } else {
            new (&this->err) auto(std::forward<R>(other).err);
        }
    }

    template <typename R>
    void assign(R&& other) {
        if (other.is_ok_) {
            if (this->is_ok_) {
                this->ok = std::forward<R>(other).ok;
            } else {
                if constexpr (!std::is_trivially_destructible_v<E>) {
                    this->err.~E();
                }
                new (&this->ok) auto(std::forward<R>(other).ok);
                this->is_ok_ = true;
            }
        } else {
            if (this->is_ok_) {
                if constexpr (!std::is_trivially_destructible_v<T>) {
                    this->ok.~T();
                }
                new (&this->err) auto(std::forward<R>(other).err);
                this->is_ok_ = false;
            } else {
                this->err = std::forward<R>(other).err;
            }
        }
    }
};

}  // namespace detail

template <typename T, typename E>
class [[nodiscard]] Result : detail::ResultBase<T, E> {
    using Base = detail::ResultBase<T, E>;

   public:
    // NOLINTNEXTLINE(*-explicit-conversions)
    Result(Ok<T>&& ok) : Base { std::move(ok) } {}

    // NOLINTNEXTLINE(*-explicit-conversions)
    Result(Err<E>&& err) : Base { std::move(err) } {}

    bool is_ok() const {
        return this->is_ok_;
    }

    bool is_err() const {
        return !this->is_ok_;
    }

    T& unwrap() & {
        return unwrap_impl(*this);
    }

    T&& unwrap() && {
        return unwrap_impl(std::move(*this));
    }

    const T& unwrap() const& {
        return unwrap_impl(*this);
    }

    const T&& unwrap() const&& {
        return unwrap_impl(std::move(*this));
    }

    E& unwrap_err() & {
        return unwrap_err_impl(*this);
    }

    E&& unwrap_err() && {
        return unwrap_err_impl(std::move(*this));
    }

    const E& unwrap_err() const& {
        return unwrap_err_impl(*this);
    }

    const E&& unwrap_err() const&& {
        return unwrap_err_impl(std::move(*this));
    }

    template <typename F>
    T unwrap_or_else(F f) const& {
        return unwrap_or_else_impl(*this, f);
    }

    template <typename F>
    T unwrap_or_else(F f) && {
        return unwrap_or_else_impl(std::move(*this), f);
    }

    T& expect(std::string_view msg) & {
        return expect_impl(*this, msg);
    }

    T&& expect(std::string_view msg) && {
        return expect_impl(std::move(*this), msg);
    }

    const T& expect(std::string_view msg) const& {
        return expect_impl(*this, msg);
    }

    const T&& expect(std::string_view msg) const&& {
        return expect_impl(std::move(*this), msg);
    }

   private:
    template <typename Self>
    static auto&& unwrap_impl(Self&& self) {
        if (CCL_UNLIKELY(self.is_err())) {
            panic("unwrap");
        }
        return std::forward<Self>(self).ok;  // NOLINT (*-union-access)
    }

    template <typename Self>
    static auto&& unwrap_err_impl(Self&& self) {
        if (CCL_UNLIKELY(self.is_ok())) {
            panic("unwrap_err");
        }
        return std::forward<Self>(self).err;
    }

    template <typename Self, typename F>
    static T unwrap_or_else_impl(Self&& self, F f) {
        return self.is_ok() ? std::forward<Self>(self).ok
                            : f(std::forward<Self>(self).err);
    }

    template <typename Self>
    static auto&& expect_impl(Self&& self, std::string_view msg) {
        if (CCL_UNLIKELY(self.is_err())) {
            panic(msg);
        }
        return std::forward<Self>(self).ok;
    }
};

namespace prelude {

using ccl::Err;
using ccl::Ok;
using ccl::Result;

}  // namespace prelude

}  // namespace ccl

#endif
