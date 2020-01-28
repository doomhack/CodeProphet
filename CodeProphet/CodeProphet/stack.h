#pragma once

#include <stdio.h>
#include <assert.h>
#include <Windows.h>

#define ROUNDUP(num,align) (((num)+((align)-1))&(~((align)-1)))

#define DEFAULT_BLOCKSIZE 8
#define DEFAULT_SHRINKBLOCKS 2


template <class T>
class CStack
{
	public:

	CStack::CStack(u32 count = 0, u32 blocksize = DEFAULT_BLOCKSIZE, u32 shrinkblocks = DEFAULT_SHRINKBLOCKS);
	CStack::~CStack();

	void Push(T& data);
	T Pop();
	T At(u32 index);
	void Reset();
	u32 Size();
	
	private:

	bool Alloc(s32 count);
	void Shrink();

	u32 size;
	u32 blocksize;
	u32 shrinkblocks;
	T *base_addr;
	T *curr_pos;
};

template <class T>
CStack<T>::CStack(u32 count = 0, u32 blocksize = DEFAULT_BLOCKSIZE, u32 shrinkblocks = DEFAULT_SHRINKBLOCKS)
{
	blocksize == 0 ? this->blocksize = 1 : this->blocksize = blocksize;
	
	this->base_addr = NULL;
	this->curr_pos = NULL;
	this->size = 0;
	this->shrinkblocks = shrinkblocks;

	if(count)
	{
		this->Alloc(count);
#ifdef DEBUG
		assert(base_addr != NULL);
#endif
	}
}

template <class T>
CStack<T>::~CStack()
{
	this->Reset();
}

template <class T>
void CStack<T>::Reset()
{
	if(this->base_addr)
	{
		free(this->base_addr);
		this->base_addr = NULL;
	}

	this->size = 0;
	this->curr_pos = NULL;
}

template <class T>
u32 CStack<T>::Size()
{
	//Important. This is not the same as this->size.
	return static_cast<u32> (this->curr_pos - this->base_addr);
}

template <class T>
void CStack<T>::Push(T& data)
{
	/*
	1) Is there any space on the stack?
	2) If not, alloc 1xblocksize.
	3) Copy data.
	4) Incriment stackpointer.
	*/

	if(!this->base_addr || this->Size() >= this->size)
		this->Alloc(1);

	*this->curr_pos++ = data;
}

template <class T>
T CStack<T>::Pop()
{
	T popvalue = (this->curr_pos > this->base_addr) ? *--this->curr_pos : *this->curr_pos;
	
	this->Shrink();

#ifdef DEBUG
	assert(this->curr_pos >= this->base_addr);
#endif
	return popvalue;
}

template <class T>
T CStack<T>::At(u32 index)
{
#ifdef DEBUG
	assert( (this->curr_pos - this->base_addr) > (s32)index);
#endif

	return this->base_addr[index];
}

template <class T>
bool CStack<T>::Alloc(s32 count)
{
	/*
	1) Add count to size.
	2) Round up to nearest blocksize.
	3) If base_addr == NULL, malloc new size.
	4) Else, realloc new size.
	5) Move base pointer to new position. base_addr may be moved.
	6) Calling with count = 0 resizes the memory pool down.
	*/

	u32 stackpos = static_cast<u32> (this->curr_pos - this->base_addr);

	if(!count)
		count = (this->Size() - this->size); //Count = 0 means relinquish free blocks at end.

	this->size = ROUNDUP(this->size + count, this->blocksize);
	
	if(!size)
	{
		free(this->base_addr);
		this->base_addr = NULL;
		this->curr_pos = NULL;
		this->size = 0;
	}
	else if(!this->base_addr)
		this->base_addr = static_cast <T*> (malloc( sizeof(T) * this->size ));
	else
		this->base_addr = static_cast<T*> (realloc(this->base_addr, sizeof(T) * this->size));

#ifdef DEBUG
	if(size)
		assert(this->base_addr != NULL);
#endif

	this->curr_pos = this->base_addr + stackpos;

	return true;
}

template <class T>
void CStack<T>::Shrink()
{
	/*
	1) Test how many free blocks at end of stack.
	2) If free blocks > shrinkblocks
		- Realloc down to size - shrinkblock.
	3) Rebase curr_pos.
	*/

	if(this->shrinkblocks)
	{
		if( (this->size - this->Size()) >= (this->shrinkblocks * blocksize) )
			this->Alloc(0);
	}
}