#ifndef RT_Circular_Buffer_H
#define RT_Circular_Buffer_H

#include <string.h>
#include "api/combase.h"
#ifndef rt_min
#define rt_min(a, b) (((a) < (b)) ? (a) : (b))
#endif

#ifndef rt_max
#define rt_max(a, b) (((a) > (b)) ? (a) : (b))
#endif

class CircularBuffer
{
public:
	CircularBuffer(uint32_t capacity)
		: beg_index_(0), end_index_(0), size_(0), capacity_(capacity)
	{
		data_ = new uint8_t[capacity];
	}

	~CircularBuffer()
	{
		if (data_)
			delete[] data_;
		data_ = NULL;
	}

	void clear()
	{
		size_ = 0;
	}

	uint32_t capacity() const
	{
		return capacity_;
	}

	uint32_t available() const
	{
		return capacity_ - size_;
	}

	uint32_t used() const
	{
		return size_;
	}

	uint32_t put_data(const uint8_t *data, uint32_t len)
	{
		if (0 == len)
			return 0;

		uint32_t capacity = capacity_;
		uint32_t bytes_to_write = _min(len, capacity - size_);

		// Write in a single step
		if (bytes_to_write <= capacity - end_index_)
		{
			memcpy(data_ + end_index_, data, bytes_to_write);
			end_index_ += bytes_to_write;
			if (end_index_ == capacity)
				end_index_ = 0;
		}
		// Write in two steps
		else
		{
			uint32_t size_1 = capacity - end_index_;
			memcpy(data_ + end_index_, data, size_1);
			uint32_t size_2 = bytes_to_write - size_1;
			memcpy(data_, data + size_1, size_2);
			end_index_ = size_2;
		}

		size_ += bytes_to_write;
		return bytes_to_write;
	}

	uint32_t get_data(uint8_t *data, uint32_t len)
	{
		if (0 == len)
			return 0;

		uint32_t capacity = capacity_;
		uint32_t bytes_to_read = _min(len, size_);

		// Read in a single step
		if (bytes_to_read <= capacity - beg_index_)
		{
			memcpy(data, data_ + beg_index_, bytes_to_read);
			beg_index_ += bytes_to_read;
			if (beg_index_ == capacity)
				beg_index_ = 0;
		}
		// Read in two steps
		else
		{
			uint32_t size_1 = capacity - beg_index_;
			memcpy(data, data_ + beg_index_, size_1);
			uint32_t size_2 = bytes_to_read - size_1;
			memcpy(data + size_1, data_, size_2);
			beg_index_ = size_2;
		}

		size_ -= bytes_to_read;
		return bytes_to_read;
	}

protected:
	inline uint32_t _max(uint32_t a, uint32_t b)
	{
		return rt_max(a, b);
	}

	inline uint32_t _min(uint32_t a, uint32_t b)
	{
		return rt_min(a, b);
	}

private:
	uint32_t beg_index_, end_index_, size_, capacity_;
	uint8_t *data_;
};

#endif // RT_Circular_Buffer_H
