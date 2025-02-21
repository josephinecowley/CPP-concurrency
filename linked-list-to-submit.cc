#include <iostream>
#include <thread>
#include <mutex>
#include <time.h>
#include <string>
#include <chrono>

struct StackItem
{
    int number;
    std::string word;
    struct StackItem *prev;
    struct StackItem *next;
    mutable std::mutex m;
};

StackItem *new_node(std::string str, int num)
{
    StackItem *node = new StackItem;
    node->word = str;
    node->number = num;
    node->prev = nullptr;
    node->next = nullptr;
    return node;
}

void push(StackItem **head, std::string node_string, int node_number)
{
    StackItem *temp = new_node(node_string, node_number);
    temp->next = *head;
    if ((*head) != NULL)
    {
        (*head)->prev = temp;
    }
    (*head) = temp;
}

void pop(StackItem **head) // not used in main()
{
    StackItem *temp = *head;

    while (temp->next != nullptr)
    {
        temp = temp->next;
    }

    temp->prev->next = nullptr;
}

std::string generate_string(int minimum_length, int maximum_length)
{
    std::string result;
    int word_length = minimum_length + rand() % (maximum_length - minimum_length + 1);

    for (int i = 0; i < word_length; i++)
    {
        char random_char = 'a' + rand() % 26;
        result.push_back(random_char);
    }

    return result;
}

void populate_stack(StackItem **head_ref, int number_of_nodes, int number_range, int minimum_length, int maximum_length)
{
    for (int i = 0; i < number_of_nodes; i++)
    {
        std::string word = generate_string(minimum_length, maximum_length);
        int number = rand() % number_range;
        push(head_ref, word, number);
    }
}

void reverse(StackItem **head_node)
{
    StackItem *new_head = NULL;
    StackItem *current = *head_node;

    while (current->next != nullptr && current != nullptr)
    {
        std::unique_lock<std::mutex> lock_1(current->m, std::defer_lock);
        std::unique_lock<std::mutex> lock_2(current->next->m, std::defer_lock);
        std::lock(lock_1, lock_2);

        push(&new_head, current->word, current->number);
        current = current->next;
    }

    push(&new_head, current->word, current->number);

    std::unique_lock<std::mutex> lock1((*head_node)->m);
    (*head_node) = new_head;
}

int calculate(StackItem *head)
{
    int result = 0;

    if (head == nullptr)
    {
        return 0;
    }
    while (head->next != nullptr)
    {
        std::unique_lock<std::mutex> lock_1(head->m, std::defer_lock);
        std::unique_lock<std::mutex> lock_2(head->next->m, std::defer_lock);
        std::lock(lock_1, lock_2);

        result += head->number;
        head = head->next;
    }

    result += head->number;

    std::string output = "Sum = " + std::to_string(result);
    std::cout << output << std::endl;

    return result;
}

void display_stack(StackItem *head)
{
    std::string output;

    while (head->next != nullptr)
    {
        std::unique_lock<std::mutex> lock_1(head->m, std::defer_lock);
        std::unique_lock<std::mutex> lock_2(head->next->m, std::defer_lock);
        std::lock(lock_1, lock_2);
        output += head->word + std::to_string(head->number) + " -> ";
        head = head->next;
    }

    output += head->word + std::to_string(head->number);

    std::cout << output << std::endl;
}

int get_length(StackItem *head)
{
    if (head == nullptr)
        return 0;

    int length = 1;

    while (head->next != nullptr)
    {
        std::unique_lock<std::mutex> lock_1(head->m, std::defer_lock);
        std::unique_lock<std::mutex> lock_2(head->next->m, std::defer_lock);
        std::lock(lock_1, lock_2);

        length += 1;
        head = head->next;
    }

    return length;
}

void delete_random(StackItem **head, int min, int max)
{

    if (*head == nullptr)
    {
        return;
    }
    int length = get_length(*head);
    int random_index = rand() % length;

    StackItem *temp = *head;

    // Traverse to the correct element in the list
    for (int i = 0; i < random_index; i++)
    {
        std::unique_lock<std::mutex> lock_1(temp->m, std::defer_lock);
        std::unique_lock<std::mutex> lock_2(temp->next->m, std::defer_lock);
        std::lock(lock_1, lock_2);
        temp = temp->next;
    }
    if (temp->next == nullptr && temp->prev == nullptr)
    {
        std::unique_lock<std::mutex> lock_1(temp->m);
        (*head) = nullptr;
        return;
    }
    else if (temp->next == nullptr)
    {
        std::unique_lock<std::mutex> lock_1(temp->prev->m, std::defer_lock);
        std::unique_lock<std::mutex> lock_2(temp->m, std::defer_lock);
        std::lock(lock_1, lock_2);
        temp->prev->next = nullptr;
    }
    else if (temp->prev == nullptr)
    {
        std::unique_lock<std::mutex> lock_1(temp->m, std::defer_lock);
        std::unique_lock<std::mutex> lock_2(temp->next->m, std::defer_lock);
        std::lock(lock_1, lock_2);
        temp->next->prev = nullptr;
    }
    else
    {
        std::unique_lock<std::mutex> lock_1(temp->prev->m, std::defer_lock);
        std::unique_lock<std::mutex> lock_2(temp->m, std::defer_lock);
        std::unique_lock<std::mutex> lock_3(temp->next->m, std::defer_lock);
        std::lock(lock_1, lock_2, lock_3);

        temp->prev->next = temp->next;
        temp->next->prev = temp->prev;
    }
}

void reverse_thread(StackItem **head_node)
{
    while (*head_node != NULL)
    {
        reverse(head_node);
        calculate(*head_node);
    }
}

void display_thread(StackItem **head_node)
{
    while (*head_node != NULL)
    {
        display_stack(*head_node);
    }
}

void delete_thread(StackItem **head_node, int min, int max, int wait_ms)
{
    while (*head_node != NULL)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(wait_ms)); // TODO change this to 100
        int num_deletes = min + rand() % (max - min + 1);

        for (int i = 0; i < num_deletes; i++)
        {
            delete_random(head_node, min, max);
        }
    }
}
int main()
{
    int number_of_items = 1024;
    int number_range = 256;
    int minimum_string_length = 8;
    int maximum_string_length = 13;
    int min_delete = 1;
    int max_delete = 3;
    int wait_ms = 100;

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

    srand(time(nullptr));

    StackItem *head_node = NULL;

    populate_stack(&head_node, number_of_items, number_range, minimum_string_length, maximum_string_length);
    std::cout << "populated stack " << std::endl;

    display_stack(head_node);

    std::thread thread_1(reverse_thread, &head_node);
    std::thread thread_2(display_thread, &head_node);
    std::thread thread_3(delete_thread, &head_node, min_delete, max_delete, wait_ms);

    thread_1.join();
    std::cout << "Thread 1 joined." << std::endl;
    thread_2.join();
    std::cout << "Thread 2 joined." << std::endl;
    thread_3.join();
    std::cout << "Thread 3 joined." << std::endl;

    return 0;
}