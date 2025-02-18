#pragma once

#include <cstddef>
#include <cstdint>
#include <limits>
#include <random>
#include <type_traits>

// Interface

namespace xoshiro256starstar {
struct seed_from_urbg_t {};
inline constexpr seed_from_urbg_t seed_from_urbg{};

struct alignas(4 * sizeof(std::uint64_t)) xoshiro256starstar {
  using result_type = std::uint64_t;

  static constexpr auto min() noexcept {
    return std::numeric_limits<std::uint64_t>::min();
  }

  static constexpr auto max() noexcept {
    return std::numeric_limits<std::uint64_t>::max();
  }

  constexpr std::uint64_t operator()() noexcept;

  // Constructors
  template <typename T>
  explicit constexpr xoshiro256starstar(seed_from_urbg_t,
                                        T &&other_urbg) noexcept;
  explicit xoshiro256starstar();
  explicit constexpr xoshiro256starstar(std::uint64_t seed0,
                                        std::uint64_t seed1,
                                        std::uint64_t seed2,
                                        std::uint64_t seed3) noexcept;
  explicit constexpr xoshiro256starstar(std::uint64_t seed) noexcept;

  // Jump functions
  constexpr xoshiro256starstar jump() noexcept;
  constexpr xoshiro256starstar long_jump() noexcept;

private:
  std::uint64_t m_state[4];
};

// Implementation

namespace detail {
constexpr inline auto rotl(const std::uint64_t x, unsigned k) noexcept {
  return (x << k) | (x >> (64u - k));
}

constexpr inline auto splitmix64(std::uint64_t &x) noexcept {
  std::uint64_t z = (x += 0x9e3779b97f4a7c15ull);
  z = (z ^ (z >> 30u)) * 0xbf58476d1ce4e5b9ull;
  z = (z ^ (z >> 27u)) * 0x94d049bb133111ebull;
  return z ^ (z >> 31u);
}

template <typename T>
constexpr inline std::uint64_t generate_uint64(T &generator) {
  using result_type = typename T::result_type;
  static_assert(std::has_unique_object_representations<result_type>::value);
  if constexpr (sizeof(result_type) >= sizeof(std::uint64_t)) {
    return std::uint64_t{generator()};
  } else {
    constexpr auto sz = sizeof(result_type);
    constexpr auto calls = (sizeof(std::uint64_t) + sz - 1) / sz;
    std::uint64_t result{};
    for (std::size_t i = 0; i != calls; ++i) {
      result = (result << (sz * CHAR_BIT)) | generator();
    }
    return result;
  }
}

constexpr inline void jmp(xoshiro256starstar &generator,
                          std::uint64_t (&state)[4],
                          const std::uint64_t (&jump_table)[4]) noexcept {
  std::uint64_t temp[4]{};
  for (auto jump_value : jump_table) {
    for (unsigned b = 0; b != 64u; ++b) {
      if (jump_value & (std::size_t(1) << b)) {
        temp[0] ^= state[0];
        temp[1] ^= state[1];
        temp[2] ^= state[2];
        temp[3] ^= state[3];
      }
      generator();
    }
  }

  state[0] = temp[0];
  state[1] = temp[1];
  state[2] = temp[2];
  state[3] = temp[3];
}
} // namespace detail

constexpr inline std::uint64_t xoshiro256starstar::operator()() noexcept {
  // -- xoshiro256** --
  const std::uint64_t result = detail::rotl(m_state[1] * 5, 7) * 9;
  const std::uint64_t temp = m_state[1] << 17u;
  m_state[2] ^= m_state[0];
  m_state[3] ^= m_state[1];
  m_state[1] ^= m_state[2];
  m_state[0] ^= m_state[3];
  m_state[2] ^= temp;
  m_state[3] = detail::rotl(m_state[3], 45u);
  return result;
}

constexpr inline xoshiro256starstar::xoshiro256starstar(
    std::uint64_t seed0, std::uint64_t seed1, std::uint64_t seed2,
    std::uint64_t seed3) noexcept
    : m_state{seed0, seed1, seed2, seed3} {}

constexpr inline xoshiro256starstar::xoshiro256starstar(
    std::uint64_t seed) noexcept
    : m_state{
          detail::splitmix64(seed),
          detail::splitmix64(seed),
          detail::splitmix64(seed),
          detail::splitmix64(seed),
      } {}

template <typename T>
constexpr inline xoshiro256starstar::xoshiro256starstar(seed_from_urbg_t,
                                                        T &&other_urbg) noexcept
    : m_state{
          detail::generate_uint64(other_urbg),
          detail::generate_uint64(other_urbg),
          detail::generate_uint64(other_urbg),
          detail::generate_uint64(other_urbg),
      } {}

inline xoshiro256starstar::xoshiro256starstar()
    : xoshiro256starstar(seed_from_urbg, std::random_device{}) {}

// This is the jump function for the generator. It is equivalent to 2^128
// calls to operator()(); it can be used to generate 2^128 non-overlapping
// subsequences for parallel computations.
constexpr inline xoshiro256starstar xoshiro256starstar::jump() noexcept {
  constexpr std::uint64_t jump_table[] = {
      0x180ec6d33cfd0abaull,
      0xd5a61266f0c9392cull,
      0xa9582618e03fc9aaull,
      0x39abdc4529b1661cull,
  };
  xoshiro256starstar save = *this;
  detail::jmp(*this, m_state, jump_table);
  return save;
}

// This is the long-jump function for the generator. It is equivalent to 2^192
// calls to operator()(); it can be used to generate 2^64 starting points, from
// each of which jump() will generate 2^64 non-overlapping subsequences for
// parallel distributed computations.
constexpr inline xoshiro256starstar xoshiro256starstar::long_jump() noexcept {
  constexpr std::uint64_t long_jump_table[] = {
      0x76e15d3efefdcbbfull,
      0xc5004e441c522fb3ull,
      0x77710069854ee241ull,
      0x39109bb02acbe635ull,
  };
  xoshiro256starstar save = *this;
  detail::jmp(*this, m_state, long_jump_table);
  return save;
}
} // namespace xoshiro256starstar
