// Note about my approach:
// This is my std::vector implementation. I also implemented a doubly linked list solution, though this was more
// complex and a correct solution would probably demand at most two background threads.
// To eliminate std::reverse() which is O(N), I tried a std::list data structure with an is_reverse() flag, but additional
// indexing times counteracted improvements.

// g++ -Wall --std=c++11 4f14_jhmdc2.cc -o 4f14

#include <iostream>
#include <vector>
#include <mutex>
#include <string>
#include <cstdlib> // for rand()
#include <time.h>
#include <thread>

std::string generate_random_string(int min_len, int max_len)
{
    std::string result;
    int rand_len = min_len + rand() % (max_len - min_len + 1); // random string length between specified values

    for (int i = 0; i < rand_len; i++)
    {
        char rand_char = 'a' + rand() % 26; // random character of 26 letter alphabet
        result.push_back(rand_char);
    }

    return result;
}

struct StackItem
{
    std::string word;
    int number;

    StackItem(std::string s, int n) : word(std::move(s)), number(n) {} // using move to avoid slow string copying
};

class ConcurrentStack
{
    std::vector<StackItem> stack;
    mutable std::mutex mutex;

public:
    void push(std::string s, int n);
    void pop();
    void populate_stack(int number_of_items, int number_range, int min_len, int max_len);
    bool reverse_and_sum();                                    // thread 1
    bool display();                                            // thread 2
    bool delete_random(int min_int, int max_int, int wait_ms); // thread 3
};

void ConcurrentStack::push(std::string s, int n)
{
    std::lock_guard<std::mutex> lock(mutex);
    stack.push_back(StackItem(std::move(s), n));
}

void ConcurrentStack::pop() // not used in main()
{
    std::lock_guard<std::mutex> lock(mutex);

    if (!stack.empty())
    {
        stack.pop_back();
    }
    else
    {
        std::cout << "Stack underflow. Cannot pop an empty stack!" << std::endl;
    }
}

void ConcurrentStack::populate_stack(int number_of_items, int number_range, int min_len, int max_len)
{
    for (int i = 0; i < number_of_items; i++)
    {
        std::string rand_word = generate_random_string(min_len, max_len);
        int rand_num = rand() % number_range; // random number between specified values
        push(std::move(rand_word), rand_num);
    }
}

bool ConcurrentStack::delete_random(int min, int max, int wait_ms)
{
    std::this_thread::sleep_for(std::chrono::milliseconds(wait_ms));
    int num_deletes = min + rand() % (max - min + 1); // randomly remove between min and max number of items

    for (int i = 0; i < num_deletes; i++)
    {
        std::unique_lock<std::mutex> lock(mutex);

        if (stack.empty())
        {
            return false;
        }

        int length = stack.size();
        int random_index = rand() % length; // choose random element to delete

        auto it = stack.begin(); // use iterators to find the element at random_index
        std::advance(it, random_index);

        stack.erase(it);
    }

    return true;
}

bool ConcurrentStack::reverse_and_sum()
{
    std::unique_lock<std::mutex> lock(mutex);

    if (stack.empty())
    {
        return false;
    }

    std::reverse(stack.begin(), stack.end());

    int sum = 0;
    for (const auto &item : stack)
    {
        sum += item.number;
    }

    std::cout << "Sum of numbers in the stack: " << sum << std::endl;

    return true;
}

bool ConcurrentStack::display()
{
    std::unique_lock<std::mutex> lock(mutex);

    if (stack.empty())
    {
        return false;
    }

    for (const auto &item : stack)
    {
        std::cout << item.word << item.number << " -> ";
    }

    std::cout << std::endl;

    return true;
}

void reverse_thread(ConcurrentStack &stack)
{
    while (stack.reverse_and_sum())
    {
        //
    }
}

void display_thread(ConcurrentStack &stack)
{
    while (stack.display())
    {
        //
    }
}

void delete_thread(ConcurrentStack &stack, int min, int max, int wait_ms)
{
    while (stack.delete_random(min, max, wait_ms))
    {
        //
    }
}

int main()
{
    const int number_of_items = 1024;
    const int number_range = 256;
    const int minimum_string_length = 8;
    const int maximum_string_length = 13;
    const int min_delete = 1;
    const int max_delete = 3;
    const int wait_ms = 100;

    // check variables are valid
    try
    {
        if (number_of_items <= 0 || number_range <= 0 || minimum_string_length <= 0 ||
            maximum_string_length <= 0 || min_delete <= 0 || max_delete <= 0 || wait_ms <= 0)
        {
            throw std::invalid_argument("All values must be positive.");
        }

        if (maximum_string_length < minimum_string_length)
        {
            throw std::invalid_argument("maximum_string_length must be greater than or equal to minimum_string_length.");
        }

        if (max_delete < min_delete)
        {
            throw std::invalid_argument("max_delete must be greater than or equal to min_delete.");
        }
    }
    catch (const std::invalid_argument &e)
    {
        std::cerr << "Invalid argument: " << e.what() << std::endl;
        return 1;
    }

    ConcurrentStack stack;

    srand(time(nullptr));

    stack.populate_stack(number_of_items, number_range, minimum_string_length, maximum_string_length);
    std::cout << "Have populated stack successfully." << std::endl;

    stack.display(); // display initial stack

    std::thread thread_1(reverse_thread, std::ref(stack));
    std::thread thread_2(display_thread, std::ref(stack));
    std::thread thread_3(delete_thread, std::ref(stack), min_delete, max_delete, wait_ms);

    thread_1.join();
    std::cout << "Thread 1 joined." << std::endl;
    thread_2.join();
    std::cout << "Thread 2 joined." << std::endl;
    thread_3.join();
    std::cout << "Thread 3 joined." << std::endl;

    return 0;
}