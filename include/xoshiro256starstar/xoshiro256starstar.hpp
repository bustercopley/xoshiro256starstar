#pragma once

#include <array>
#include <concepts>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <limits>
#include <random>

namespace xoshiro256starstar {
struct seed_from_urbg_t {};

inline constexpr seed_from_urbg_t seed_from_urbg{};

struct alignas(32) xoshiro256starstar {
  using result_type = std::uint64_t;

  static constexpr auto min() {
    return std::numeric_limits<std::uint64_t>::min();
  }

  static constexpr auto max() {
    return std::numeric_limits<std::uint64_t>::max();
  }

  inline constexpr std::uint64_t operator()();

  // Constructors
  template <typename T>
  xoshiro256starstar(seed_from_urbg_t, T &&other_urbg) requires
      std::uniform_random_bit_generator<std::remove_cvref_t<T>>;
  explicit xoshiro256starstar();
  explicit xoshiro256starstar(std::uint64_t seed);
  explicit xoshiro256starstar(const std::array<std::byte, 256> &seed);

  // Jump functions
  xoshiro256starstar jump();
  xoshiro256starstar long_jump();

private:
  std::uint64_t m_state[4];
};

namespace detail {
inline constexpr auto rotl(const std::uint64_t x, unsigned k) {
  return (x << k) | (x >> (64u - k));
}

inline auto splitmix64(std::uint64_t &x) {
  std::uint64_t z = (x += 0x9e3779b97f4a7c15ull);
  z = (z ^ (z >> 30u)) * 0xbf58476d1ce4e5b9ull;
  z = (z ^ (z >> 27u)) * 0x94d049bb133111ebull;
  return z ^ (z >> 31u);
}

inline void jmp(xoshiro256starstar &generator, std::uint64_t (&state)[4],
                const std::uint64_t (&jump_table)[4]) {
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

inline constexpr std::uint64_t xoshiro256starstar::operator()() {
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

inline xoshiro256starstar::xoshiro256starstar(
    const std::array<std::byte, 256> &seed) {
  std::memcpy(m_state, &seed, sizeof m_state);
}

inline xoshiro256starstar::xoshiro256starstar(std::uint64_t seed) {
  for (auto &x : m_state) {
    x = detail::splitmix64(seed);
  }
}

inline xoshiro256starstar::xoshiro256starstar()
    : xoshiro256starstar(seed_from_urbg, std::random_device{}) {}

template <typename T>
inline xoshiro256starstar::xoshiro256starstar(seed_from_urbg_t,
                                              T &&other_urbg) requires
    std::uniform_random_bit_generator<std::remove_cvref_t<T>> {

  using other_result_type = typename std::remove_cvref_t<T>::result_type;
  constexpr auto bytes_per_call = sizeof(other_result_type);
  constexpr auto calls = (sizeof m_state + bytes_per_call - 1) / bytes_per_call;
  other_result_type seed[calls];
  for (auto &value : seed) {
    value = other_urbg();
  }
  std::memcpy(&m_state, &seed, sizeof m_state);
}

// This is the jump function for the generator. It is equivalent to 2^128
// calls to operator()(); it can be used to generate 2^128 non-overlapping
// subsequences for parallel computations.
inline xoshiro256starstar xoshiro256starstar::jump() {
  static const std::uint64_t jump_table[] = {
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
inline xoshiro256starstar xoshiro256starstar::long_jump() {
  static const std::uint64_t long_jump_table[] = {
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
