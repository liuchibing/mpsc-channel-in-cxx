// Copyright PA Knowledge 2023

#include <mpsc/channel.hpp>

#include <cassert>

auto main(int argc, char **argv) -> int {
    // Create.
    auto [sender, receiver] = mpsc::make_channel<int>();

    // Send.
    sender.send(3);

    // Receive (both returns a std::optional<T>.)
    receiver.receive(); // Blocking when there is nothing present in the channel.
    receiver.try_receive(); // Not blocking. Return immediately.

    // close() and closed()
    sender.close();
    bool result = sender.closed();
    assert(result == receiver.closed());

    // You can use range-based for loop to receive from the channel.
    for (int v: receiver) {
        // do something with v
        // The loop will stop immedately after the sender called close().
        // Only sender can call close().
    }

    return 0;
}
