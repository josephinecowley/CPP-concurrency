# CPP-concurrency: assignment

The code should consist of exactly one C++ file with the name `4f14_crsid.cc` (where “crsid” is your CRSid, e.g. “pok21”). No external libraries beyond the C++ standard library are allowed. Your code must compile correctly using the following command on a modern GCC compiler:

```sh
g++ -Wall --std=c++11 4f14_crsid.cc -o 4f14
```

## Requirements

#### a. Queue Implementation

- Implement a queue data structure that holds items that can be added and removed from the queue.
- Each item in the queue should consist of:
  - A single `std::string`
  - A single integer.
- The queue should support:
  - Adding an item to the queue.
  - Removing an item from the queue.
- A queue works like a stack, except for removing an item:
  - In a **stack**, when an item is removed, the **last** item added is removed (**LIFO**).
  - In a **queue**, when an item is removed, the **first** item added is removed (**FIFO**).

#### b. Queue Population

- Instantiate your queue and implement a method to populate it with **80 items**.
- Each item should have:
  - A string of **3-7 characters**, chosen randomly.
  - Each character should be a letter between **'a' and 'z'** (inclusive), chosen randomly.
  - An integer between **0 and 255** (inclusive), chosen randomly.
- This method should be executed **once** in the `main` function immediately after queue creation.

#### c. Background Thread - Reversing & Summing

- Start a background thread that continually:
  - Reverses the order of all items in the queue.
  - Outputs the **sum** of all integer values in the queue **at the time of reversal**.
- The thread should repeat this process as long as there are items in the queue.
- When the queue is empty, the thread should stop.
- Consider adding a **small delay** to facilitate debugging.

#### d. Background Thread - Printing Queue Contents

- Start another background thread that continually:
  - Prints out all **string and integer values** of the items currently in the queue.
- The thread should **stop running when the queue is empty**.
- Consider adding a **small delay** to prevent console overload.

#### e. Background Thread - Random Removal

- Start a third background thread that:
  - Every **0.2 seconds**, randomly selects an item from the queue.
  - Removes the selected item from the queue.
- The thread should **stop running when the queue is empty**.
