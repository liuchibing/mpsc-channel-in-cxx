# mpsc channel in c++

channel.hpp - A simple C++ implementation of the 'channel' in rust language (std::mpsc).

# Usage
Use `make_channel<T>` to create a channel. `make_channel<T>` will return a tuple of (Sender<T>, Receiver<T>).

For example:
```c++
// Create.
auto [ sender, receiver ] = make_channel<int>();

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
```

Note: `mpsc` stands for Multi-Producer Single-Consumer. So `Sender` can be either copied and moved, but `Receiver` can only be moved.

Feel free to explore the `tests.cpp`. The tests are also examples of the usage.

Read the source if you need more information. Sorry for the lack of comments. ～(￣▽￣～)~

Copyright (c) 2019 liuchibing.
