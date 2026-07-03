#include <cstddef>
#include <string>

#ifndef DARRAY_H
#define DARRAY_H

class DArray
{
   private:
    std::string *listMusic;
    size_t capacity = 1;
    size_t currentSize = 0;
    void reSize();

   public:
    ~DArray();
    DArray();

    void push(std::string element);
    std::string access(size_t index) const;
    std::size_t size() const;


    //Cannot Create Copy Of This
    DArray(const DArray &) = delete;
    DArray &operator=(const DArray &) = delete;
};

#endif
