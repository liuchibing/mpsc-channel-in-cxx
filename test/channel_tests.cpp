// Copyright PA Knowledge 2023

#include <mpsc/channel.hpp>

#include <catch2/catch_test_macros.hpp>

#include <random>
#include <algorithm>
#include <vector>

TEST_CASE("Channel tests") {
  auto rng = std::mt19937_64{7654236};  // Arbitrary seed.

  SECTION("Trivially constructable values") {
    auto [tx, rx] = mpsc::make_channel<int>();

    SECTION("A single value can be sent and received") {
      auto sent_value = std::uniform_int_distribution<int>{}(rng);
      tx.send(sent_value);
      const auto rcvd = rx.receive();

      REQUIRE(rcvd.has_value());
      REQUIRE(sent_value == rcvd.value());
    }

    SECTION("Multiple values can be sequentially sent and received") {
      for (auto i = 0; i < 10; ++i) {
        auto sent_value = std::uniform_int_distribution<int>{}(rng);
        tx.send(sent_value);
        const auto rcvd = rx.receive();

        REQUIRE(rcvd.has_value());
        REQUIRE(sent_value == rcvd.value());
      }
    }

    SECTION("Multiple values can be sent and then received later"){
      for (int i : std::ranges::iota_view{0, 10}) {
        tx.send(i);
      }

      auto vals = std::vector<int>{};
      std::copy_n(rx.begin(), 10, std::back_inserter(vals));

      REQUIRE(std::vector{0, 1, 2, 3, 4, 5, 6, 7, 8, 9} == vals);
    }
  }
}
