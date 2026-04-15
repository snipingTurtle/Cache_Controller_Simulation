#include<bits/stdc++.h>

using namespace std;

typedef struct
{
    int address;
    bool is_write;
    int data = 0;
}Input;

class Block
{
private:
    int tag;
    int data;
    bool valid_bit;
    bool dirty_bit;
public:
    Block() : tag(0), data(0), valid_bit(false), dirty_bit(false) {}
    Block(Input input)
    {
        tag = input.address >> 5; // Extracting the tag by shifting right by 5 bits
        data = input.data;
        valid_bit = true;
        dirty_bit = input.is_write; // Set dirty bit if it's a write operation
    }

    int get_tag() const { return tag; }
    int get_data() const { return data; }
    bool is_valid() const { return valid_bit; }
    bool is_dirty() const { return dirty_bit; } 

    void set_data(int new_data)
    {
        data = new_data;
        dirty_bit = true; // Mark as dirty when data is updated
    }

    void set_dirty(bool dirty) { dirty_bit = dirty; }
    void set_valid(bool valid) { valid_bit = valid; }
    void set_tag(int new_tag) { tag = new_tag; }
};

class Cache
{
private:
    Block blocks[32];
public:
    Cache()
    {
        for (int i = 0; i < 32; i++)
        {
            blocks[i] = Block(); // Initialize all blocks
        }
    }

    class cleanException
    {};

    class dirtyException
    {};

    int read_data(int address)
    {
        int index = address & 31;
        int tag = address >> 5;

        if (blocks[index].is_valid() && blocks[index].get_tag() == tag)
        {
            return blocks[index].get_data();
        }
        else
        {
            if(blocks[index].is_valid() && blocks[index].is_dirty())
            {
                throw dirtyException();
            }
            else
            {
                throw cleanException();
            }
        }
    }

    void update_block(Input input)
    {
        int index = input.address & 31;
        int tag = input.address >> 5;

        blocks[index].set_tag(tag);
        blocks[index].set_data(input.data);
        blocks[index].set_valid(true);
        blocks[index].set_dirty(input.is_write); // Set dirty bit based on whether it's a write operation
    }
};

class Memory
{
private:
    int data[1024]; // Simulating memory with 1024 integers
public:
    Memory()
    {
        for (int i = 0; i < 1024; i++)
        {
            data[i] = rand() % 10000; // Initialize all memory locations to a random value
        }
    }

    int read(int address)
    {
        return data[address];
    }

    void write(int address, int value)
    {
        data[address] = value;
    }
};   

class cache_simulator
{
private:
    Cache cache;
    Memory memory;
    int state = 0;
    // Idle - 0
    // Compare Tag - 1
    // Write Back - 2
    // Allocate - 3
public:
    void run()
    {
        while (true)
        {
            cout << "Current State: " << get_state() << endl;

            Input input;
            cout << "Enter operation (0 for read, 1 for write, 2 to quit)";
            cin >> input.is_write;

            if(input.is_write == 2)
            {
                cout << "Exiting simulator." << endl;
                break;
            }

            cout << "Enter address: ";
            cin >> input.address;

            if(input.address < 0 || input.address >= 1024)
            {
                cout << "Invalid address. Please enter an address between 0 and 1023." << endl;
                continue;
            }
            
            if (input.is_write)
            {
                cout << "Enter data to write: ";
                cin >> input.data;
            }
            
            state = 1; // Move to Compare Tag state
            cout << "Current State: " << get_state() << endl;

            if(input.is_write)
            {

            }
            else 
            {
                try
                {
                    int data = cache.read_data(input.address);
                    cout << "Data read from cache: " << data << endl;
                    state = 0; // Move back to Idle state
                }
                catch (Cache::dirtyException&)
                {
                    state = 2; // Move to Write Back state
                    cout << "Current State: " << get_state() << endl;
                    // Handle write back logic here
                }
                catch (Cache::cleanException&)
                {
                    state = 3; // Move to Allocate state
                    cout << "Current State: " << get_state() << endl;

                    cout << "Fetching data from memory..." << endl;
                    this_thread::sleep_for(chrono::seconds(1)); // Simulate memory access delay
                    input.data = memory.read(input.address); // Read data from memory

                    cout << "Updating cache block..." << endl;
                    this_thread::sleep_for(chrono::seconds(1)); // Simulate cache update delay
                    cache.update_block(input); // Update cache block with new data

                    cout << "Data read from memory and updated in cache: " << input.data << endl;
                    state = 0; // Move back to Idle state
                }
            }
        }
    }

    string get_state() const
    {
        switch (state)
        {
            case 0: return "Idle";
            case 1: return "Compare Tag";
            case 2: return "Write Back";
            case 3: return "Allocate";
            default: return "Unknown State";
        }
    }
};

int main()
{

}