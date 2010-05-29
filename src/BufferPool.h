#ifndef _BUFFERPOOL_H
#define _BUFFERPOOL_H

/* buffer pool experiment */
class BufferPool : public Singleton<BufferPool>
{
public:
	BufferPool();
	~BufferPool();

	void init();

	void* GetBuffer(size_t size);
	void DumpBuffer(void* buffer, size_t size);
	void ResizeBuffer(void** buffer, size_t oldsize, size_t newsize, bool copy = false);
	void print_stats();
protected:
private:

	FastQueue<void*,DummyLock> mSmallPool; /* buffer from 0x0 - 0x100 */
	FastQueue<void*,DummyLock> mMediumPool; /* buffer from 0x100 - 0x1000 */
	FastQueue<void*,DummyLock> mBigPool; /* buffer from 0x1000 - 0xFFFFFFFF */

	size_t mAllocatedMem; // keeps track of the total allocated mem of the buffer pool.
	size_t mSmallRecycleCount;
	size_t mMediumRecycleCount;
	size_t mLargeRecycleCount;

	size_t mSmallBufferCount;
	size_t mMediumBufferCount;
	size_t mLargeBufferCount;
};

// buffer pool experiment
#define sBufferPool BufferPool::getSingleton()

#endif // _BUFFERPOOL_H