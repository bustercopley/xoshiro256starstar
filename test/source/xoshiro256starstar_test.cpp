#include <algorithm>
#include <array>
#include <bit>
#include <iostream>
#include <iterator>

#include "xoshiro256starstar/xoshiro256starstar.hpp"

const std::array<uint64_t, 4> expected_sequences[]{
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
  using xoshiro256starstar::state_byte_size;

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

  return !std::ranges::all_of(
      std::array{
          sub_test(0, generator0),
          sub_test(1, generator1),
          sub_test(2, generator2),
          sub_test(3, generator3),
          sub_test(0, generator4),
          sub_test(4, generator4),
          sub_test(5, urbg{0xdeadbeeffeedbadbull}),
      },
      [](auto ok) { return ok; });
}
