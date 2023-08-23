/*
 * Copyright (c) 2023
 *
 * This software is licensed under the MIT License.
 * SPDX-License-Identifier: MIT
 *
 */
#include <mpsc/channel.hpp>

#include <catch2/catch_test_macros.hpp>

#include <random>
#include <algorithm>
#include <vector>
#include <future>
#include <condition_variable>
#include <chrono>

using namespace std::chrono_literals;

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

    SECTION("Closing sender ends receiver iterator wait") {
      auto receiving_now = std::condition_variable{};
      auto mtx = std::mutex{};
      auto lock = std::unique_lock<std::mutex>{mtx};

      auto recvd = std::vector<int>{};
      tx.send(12);

      auto async_recv = std::async([&](){
        receiving_now.notify_all();
        std::copy(rx.begin(), rx.end(), std::back_inserter(recvd));
      });

      receiving_now.wait(lock);

      // Wait for a short time to ensure that the available items are processed
      std::this_thread::sleep_for(10ms);

      // Closing the transmitter should terminate the receiver.
      tx.close();

      REQUIRE(std::vector{ 12 } == recvd);
      REQUIRE(std::future_status::ready == async_recv.wait_for(1s));
    }

    SECTION("Channel is closed when its last sender goes out of scope") {
        auto [tx_1, rx_1] = mpsc::make_channel<double>();
        {
            auto _1 = tx_1;  // This copy keeps the channel alive while it's in scope.
            {
                auto _2 = _1;
                {
                    auto tx_to_kill = std::move(tx_1);
                }
                REQUIRE_FALSE(rx_1.closed());
            }
            REQUIRE_FALSE(rx_1.closed());
        }
        REQUIRE(rx_1.closed());
    }

    SECTION("Closing a sender manually doesn't mess up scope-based channel closing") {
            auto [tx_1, rx_1] = mpsc::make_channel<double>();
            {
                auto tx_2 = tx_1;  // This copy keeps the channel alive while it's in scope.
                {
                    auto tx_3 = tx_2;
                    {
                        auto tx_to_kill = std::move(tx_1);
                    }
                    REQUIRE_FALSE(rx_1.closed());

                    tx_3.close();
                }
                REQUIRE_FALSE(rx_1.closed());
            }
            REQUIRE(rx_1.closed());
    }
  }
}
