#ifndef HASH_TABLE_HPP
#define HASH_TABLE_HPP

//=======================//
// Implemented Functions //
//=======================//
template<int MAX_SIZE>
int HashTable<MAX_SIZE>::PRIMES[3] = {102523, 100907, 104659};

template<int MAX_SIZE>
void HashTable<MAX_SIZE>::PrintLine(int tableIndex) const
{
    const HashData& data = table[tableIndex];

    // Using printf here it is easier to format
    if(data.sentinel == SENTINEL_MARK)
    {
        printf("[%03d]         : SENTINEL\n", tableIndex);
    }
    else if(data.sentinel == EMPTY_MARK)
    {
        printf("[%03d]         : EMPTY\n", tableIndex);
    }
    else
    {
        printf("[%03d] - [%03d] : ", tableIndex, data.lruCounter);
        printf("(%-5s) ", data.isCostWeighted ? "True" : "False");
        size_t sz = data.intArray.size();
        for(size_t i = 0; i < sz; i++)
        {
            if(i % 2 == 0)
                printf("[%03d]", data.intArray[i]);
            else
                printf("/%03d/", data.intArray[i]);

            if(i != sz - 1)
                printf("-->");
        }
        printf("\n");
    }
}

template<int MAX_SIZE>
void HashTable<MAX_SIZE>::PrintTable() const
{
    printf("____________________\n");
    printf("Elements %d\n", elementCount);
    printf("[IDX] - [LRU] | DATA\n");
    printf("____________________\n");
    for(int i = 0; i < MAX_SIZE; i++)
    {
        PrintLine(i);
    }
}

//=======================//
//          TODO         //
//=======================//

template<int MAX_SIZE>
int HashTable<MAX_SIZE>::QuadraticProbing(unsigned int key) {
    
    for (int i = 0; i*i < MAX_SIZE; ++i) {
        int index = (key + i * i) % MAX_SIZE;
        if (table[index].sentinel != OCCUPIED_MARK) {
            
            return index;
        }
    }

    return -1;
}

template<int MAX_SIZE>
int HashTable<MAX_SIZE>::Hash(int startInt, int endInt, bool isCostWeighted)
{
    int hashcode = PRIMES[0]*startInt + PRIMES[1]*endInt;
    if(isCostWeighted) hashcode += PRIMES[2];
    return hashcode;
}

template<int MAX_SIZE>
HashTable<MAX_SIZE>::HashTable()
{
    for(int i = 0; i < MAX_SIZE; i++){
        table[i].sentinel = EMPTY_MARK;
        table[i].lruCounter = 0;
    }
    elementCount = 0;
}

template<int MAX_SIZE>
int HashTable<MAX_SIZE>::Insert(const std::vector<int>& intArray, bool isCostWeighted)
{
    if(intArray.empty()) throw InvalidTableArgException();
    int hashcode = Hash(intArray.front(), intArray.back(), isCostWeighted);
    int index;
    for (int i = 0; i*i < MAX_SIZE; ++i) {
        int index = (hashcode + i*i) % MAX_SIZE;
        if (table[index].sentinel == OCCUPIED_MARK &&
            intArray.front() == table[index].intArray.front() &&
            intArray.back() == table[index].intArray.back() &&
            table[index].isCostWeighted == isCostWeighted) {
            
            ++(table[index].lruCounter);
            return table[index].lruCounter;
        }
    }
    
    

    if (elementCount > (float)(MAX_SIZE)/2.0) {
        throw TableCapFullException(elementCount);
    }

    index = QuadraticProbing(hashcode);

    if(index != -1){
        table[index].intArray = intArray;
        table[index].isCostWeighted = isCostWeighted;
        table[index].startInt = intArray.front();
        table[index].endInt = intArray.back();
        table[index].lruCounter = 1;  
        table[index].sentinel = OCCUPIED_MARK;
        elementCount++;
    }

    return 0;
}

template<int MAX_SIZE>
bool HashTable<MAX_SIZE>::Find(std::vector<int>& intArray,
                               int startInt, int endInt, bool isCostWeighted,
                               bool incLRU)
{
    int hashcode = Hash(startInt, endInt, isCostWeighted);

    for (int i = 0; i * i < MAX_SIZE; ++i) {
        int index = (hashcode + i * i) % MAX_SIZE;
        if (table[index].sentinel == OCCUPIED_MARK &&
            startInt == table[index].startInt &&
            endInt == table[index].endInt &&
            isCostWeighted == table[index].isCostWeighted) {

            if (incLRU) {
                table[index].lruCounter++;
            }
            intArray = table[index].intArray;

            return true;
        }
    }
    return false;
}

template<int MAX_SIZE>
void HashTable<MAX_SIZE>::InvalidateTable()
{
    for(int i = 0; i < MAX_SIZE; i++){
        if(table[i].sentinel != EMPTY_MARK){
            table[i].sentinel = EMPTY_MARK;
            table[i].intArray.clear();
            table[i].lruCounter = 0;
            
        }
    }
    elementCount = 0;
    return;
}

template<int MAX_SIZE>
void HashTable<MAX_SIZE>::GetMostInserted(std::vector<int>& intArray) const
{
    int mi_index = 0;
    int mostnm = 0;
    for(int i = 0; i < MAX_SIZE; i++){
        if(table[i].sentinel == OCCUPIED_MARK){
            if(mostnm < table[i].lruCounter){
                mostnm = table[i].lruCounter;
                mi_index = i;
            }
        }
    }
    intArray = table[mi_index].intArray;
}

template<int MAX_SIZE>
void HashTable<MAX_SIZE>::Remove(std::vector<int>& intArray,
                                 int startInt, int endInt, bool isCostWeighted)
{
    int hashcode = Hash(startInt, endInt, isCostWeighted);

    for (int i = 0; i * i < MAX_SIZE; ++i) {
        int index = (hashcode + i * i) % MAX_SIZE;
        if (table[index].sentinel == OCCUPIED_MARK &&
            startInt == table[index].startInt &&
            endInt == table[index].endInt &&
            isCostWeighted == table[index].isCostWeighted) {

            // Copy the data to intArray before removing the entry
            intArray = table[index].intArray;

            // Remove the entry from the hash table
            table[index].sentinel = SENTINEL_MARK;
            
            elementCount--;

            return;
        }
    }

    // Entry not found, do not modify intArray
}

template<int MAX_SIZE>
void HashTable<MAX_SIZE>::RemoveLRU(int lruElementCount)
{
    
        
    
    for(int i = 0; i < MAX_SIZE,lruElementCount > 0; i++){
        if(table[i].sentinel == OCCUPIED_MARK){
                table[i].sentinel = SENTINEL_MARK;
                elementCount--;
                lruElementCount--;
            }
        }
}


template<int MAX_SIZE>
void HashTable<MAX_SIZE>::PrintSortedLRUEntries() const
{
    /* TODO */
    MaxPairHeap<int, HashData> datapq;
    for(int i = 0; i < MAX_SIZE; i++){
        if(table[i].sentinel == OCCUPIED_MARK) datapq.push({table[i].lruCounter, table[i]});
    }
    int index;
    while(!datapq.empty()){
        Pair<int, HashData> top_pair = datapq.top();
        const HashData wanted = top_pair.value;
        int hashcode = Hash(wanted.startInt, wanted.endInt, wanted.isCostWeighted) ;
        
        for(int i = 0; i*i < MAX_SIZE; i++){
            index = (hashcode + i * i) % MAX_SIZE;
            if(table[index].intArray == wanted.intArray && table[index].isCostWeighted == wanted.isCostWeighted) break;
        }
        
        
        
        PrintLine(index);
        datapq.pop();

    }
}

#endif // HASH_TABLE_HPP