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
    {
        int dirty_block_data;
        int dirty_block_address;
    public:
        dirtyException(int data = 0, int address = 0) : dirty_block_data(data), dirty_block_address(address) {}
        int get_dirty_data() const { return dirty_block_data; }
        int get_dirty_address() const { return dirty_block_address; }
    };

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
                throw dirtyException(blocks[index].get_data(), (blocks[index].get_tag() << 5 | index)); // Pass the dirty block's data and address to the exception
            }
            else
            {
                throw cleanException();
            }
        }
    }

    void write_data(Input input)
    {
        int index = input.address & 31;
        int tag = input.address >> 5;

        if (blocks[index].is_valid() && blocks[index].get_tag() == tag)
        {
            blocks[index].set_data(input.data);
        }
        else
        {
            if(blocks[index].is_valid() && blocks[index].is_dirty())
            {
                throw dirtyException(blocks[index].get_data(), (blocks[index].get_tag() << 5 | index)); // Pass the dirty block's data and address to the exception
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
            int option;
            cout << "Enter operation (0 for read, 1 for write, 2 to quit) : ";
            cin >> option;

            if(option == 2)
            {
                cout << "Exiting simulator." << endl;
                break;
            }

            input.is_write = (option == 1);

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
                while(true)
                {
                    try
                    {
                        this_thread::sleep_for(chrono::seconds(1)); // Simulate cache access delay
                        cache.write_data(input);
                        cout << "Data written to cache." << endl;
                        state = 0; // Move back to Idle state
                        break;
                    }
                    catch (Cache::dirtyException& e)
                    {
                        // Handle write back logic here
                        write_back_state(input, e.get_dirty_data(), e.get_dirty_address());
                    }
                    catch (Cache::cleanException&)
                    {
                        allocate_state(input); // Move to Allocate state and handle allocation
                    }
                }
                
            }
            else 
            {
                try
                {
                    this_thread::sleep_for(chrono::seconds(1)); // Simulate cache access delay
                    int data = cache.read_data(input.address);
                    cout << "Data read from cache: " << data << endl;
                    state = 0; // Move back to Idle state
                }
                catch (Cache::dirtyException& e)
                {
                    // Handle write back logic here
                    write_back_state(input, e.get_dirty_data(), e.get_dirty_address());

                    cout << "Data read from memory and updated in cache: " << cache.read_data(input.address) << endl;
                    state = 0; // Move back to Idle state
                }
                catch (Cache::cleanException&)
                {
                    allocate_state(input); // Move to Allocate state and handle allocation

                    cout << "Data read from memory and updated in cache: " << cache.read_data(input.address) << endl;
                    state = 0; // Move back to Idle state
                }
            }
        }
    }

    void allocate_state(Input input)
    {
        state = 3; // Move to Allocate state
        cout << "Current State: " << get_state() << endl;

        this_thread::sleep_for(chrono::seconds(1)); // Simulate cache access delay

        cout << "Fetching data from memory..." << endl;
        this_thread::sleep_for(chrono::seconds(1)); // Simulate memory access delay
        input.data = memory.read(input.address); // Read data from memory

        cout << "Updating cache block..." << endl;
        this_thread::sleep_for(chrono::seconds(1)); // Simulate cache update delay
        cache.update_block(input); // Update cache block with new data

        state = 1;
        cout << "Current State: " << get_state() << endl;
    }

    void write_back_state(Input input, int dirty_data, int dirty_address)
    {
        state = 2; // Move to Write Back state
        cout << "Current State: " << get_state() << endl;

        this_thread::sleep_for(chrono::seconds(1)); // Simulate cache access delay

        cout << "Writing back dirty block to memory..." << endl;
        this_thread::sleep_for(chrono::seconds(1)); // Simulate write back delay
        memory.write(dirty_address, dirty_data); // Write back dirty block to memory

        allocate_state(input); // After write back, move to Allocate state to fetch new data
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
    cache_simulator simulator;
    simulator.run();
    return 0;
}