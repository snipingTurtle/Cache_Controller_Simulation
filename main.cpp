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

    void take_input(Input input)
    {
        int addr = input.address;
        int index = addr & 31; // Extracting the last 5 bits for index
    }
};

int main()
{

}