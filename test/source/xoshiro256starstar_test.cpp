#include <algorithm>
#include <array>
#include <bit>
#include <iostream>
#include <iterator>

#include "xoshiro256starstar/xoshiro256starstar.hpp"

constexpr std::array<std::array<uint64_t, 4>, 7> expected_sequences{{
    {
        13441579124568109284ull,
        16706691377286368614ull,
        6044645330207567355ull,
        1028949623841091013ull,
    },
    {
        12156030441243319403ull,
        15440195002139279164ull,
        621127951814214240ull,
        12743354370792161826ull,
    },
    {
        15891181023933456327ull,
        17338517592901170603ull,
        6477423152056636265ull,
        5845344969495837556ull,
    },
    {
        15098849909276232933ull,
        3979698662854242369ull,
        15367718349835638978ull,
        12310398284360036637ull,
    },
    {
        13441579124568109284ull,
        16706691377286368614ull,
        6044645330207567355ull,
        1028949623841091013ull,
    },
    {
        17233280674010829842ull,
        2397106501455141870ull,
        4812098391647980233ull,
        13206288874623648473ull,
    },
    {
        12247022307613036767ull,
        6189883159010878364ull,
        12250510543705388769ull,
        15376822593055763089ull,
    },
}};

template <typename URBG>
constexpr auto generate_sequence(URBG &&generator) noexcept {
  return std::array{generator(), generator(), generator(), generator()};
}

template <typename... URBGs>
constexpr auto generate_sequences(URBGs &&...generators) noexcept {
  return std::array{generate_sequence(generators)...};
}

constexpr auto make_actual_test_sequences() noexcept {
  using urbg = xoshiro256starstar::xoshiro256starstar;
  using xoshiro256starstar::seed_from_urbg;

  auto generator0 = urbg{
      0xdeadbeeffeedbadbull,
      0xbadfacedbeedeadfull,
      0xdadaddedbaddeedcull,
      0xdecadefadedfacedull,
  };

  auto generator1 = generator0.long_jump();
  auto generator2 = generator0.jump();
  auto generator3 = urbg{seed_from_urbg, generator0};
  auto generator4 = generator0;

  return generate_sequences(generator0, generator1, generator2, generator3,
                            generator4, generator4,
                            urbg{0xdeadbeeffeedbadbull});
}

int main() {
  // Output an unpredictable number from a default-constructed generator
  // just to make sure it compiles
  using urbg = xoshiro256starstar::xoshiro256starstar;
  using xoshiro256starstar::seed_from_urbg;
  urbg generator;
  std::cout << "Unpredictable: " << generator() << "\n";

  // By default the tests are run at compile time (except on g++ 9)
#if !__clang__ && __GNUC__ >= 10
  if constexpr (true) {
    static_assert(make_actual_test_sequences() == expected_sequences);
    return 0;
  }
#endif

  // It can be useful to see the actual sequences if the tests fail
  auto actual_test_sequences = make_actual_test_sequences();
  bool ok = actual_test_sequences == expected_sequences;
  if (!ok) {
    for (const auto &sequence : actual_test_sequences) {
      std::cout << "{";
      std::copy(std::begin(sequence), std::end(sequence),
                std::ostream_iterator<std::uint64_t>(std::cout, "ull, "));
      std::cout << "},\n";
    }
  }
  return ok ? 0 : 1;
}
