#include <algorithm>
#include <array>
#include <bit>
#include <iostream>
#include <iterator>

#include "xoshiro256starstar/xoshiro256starstar.hpp"

const std::array<uint64_t, 4> expected_sequences[]{
    {
        13346302149916581598ull,
        3458137911937826359ull,
        15013289007574211850ull,
        862047879559611952ull,
    },
    {
        12247022307613036767ull,
        6189883159010878364ull,
        12250510543705388769ull,
        15376822593055763089ull,
    },
    {
        11920099415707066346ull,
        14422800180009765440ull,
        11058914709412537623ull,
        3686108977655562628ull,
    },
    {
        14646694691296698890ull,
        15099502338332193061ull,
        12418408594807792822ull,
        1546099311609212045ull,
    },
    {
        8965407117812691042ull,
        6229718707537460892ull,
        17965388879194096341ull,
        2509736941055013603ull,
    },
    {
        561345601117264966ull,
        15261714419321652613ull,
        12890482397767998458ull,
        14696349688785942750ull,
    },
};

auto sub_test(auto sequence_number, auto &&generator) {
  static auto test_number = 0;
  auto n = test_number++;
  auto expected_sequence = expected_sequences[sequence_number];
  decltype(expected_sequence) actual_sequence;

  std::ranges::generate(actual_sequence, std::ref(generator));
  auto ok = actual_sequence == expected_sequence;

  if (!ok) {
    std::cout << "Sub-test " << n << " (sequence " << sequence_number
              << ")\nExpected {";
    std::ranges::copy(expected_sequence,
                      std::ostream_iterator<std::uint64_t>(std::cout, "ull, "));
    std::cout << "}\nActual {";
    std::ranges::copy(actual_sequence,
                      std::ostream_iterator<std::uint64_t>(std::cout, "ull, "));
    std::cout << "}\n";
  }

  return ok;
}

int main() {
  using urbg = xoshiro256starstar::xoshiro256starstar;
  using xoshiro256starstar::seed_from_urbg;

  auto generator0 = urbg{0xdeadbeeffeedbadbull};
  auto generator1 = generator0.long_jump();
  auto generator2 = generator0.jump();
  auto generator3 = urbg{seed_from_urbg, generator0};
  auto generator4 = generator0;

  const char(&seed_string)[256] =
      "Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do "
      "eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut e"
      "nim ad minim veniam, quis nostrud exercitation ullamco laboris n"
      "isi ut aliquip ex ea commodo consequat. Duis aute irure dolor i";

#if __cpp_lib_bit_cast >= 201806L
  auto seed = std::bit_cast<std::array<std::byte, 256>>(seed_string);
#else
  std::array<std::byte, 256> seed;
  std::memcpy(&seed, seed_string, sizeof seed);
#endif

  return !std::ranges::all_of(
      std::array{
          sub_test(0, generator0),
          sub_test(1, generator1),
          sub_test(2, generator2),
          sub_test(3, generator3),
          sub_test(0, generator4),
          sub_test(4, generator4),
          sub_test(5, urbg{seed}),
      },
      [](auto ok) { return ok; });
}
