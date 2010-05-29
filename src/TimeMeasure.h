#ifndef _TIMEMEASURE_H
#define _TIMEMEASURE_H

class TimeMeasure
{
public:
	TimeMeasure()
	{
		mTimeStamp = GetTickCount();
	}

	void tick()
	{
		mTimeStamp = GetTickCount();
	}

	static void printTimeDiff(TimeMeasure & time)
	{
		DWORD timeDiff = GetTickCount() - time.getTick();
		printf("TimeMeasure diff: %u ms\n", timeDiff);
	}

	uint32 getTick()
	{
		return mTimeStamp;
	}
private:
	DWORD mTimeStamp;
};
#endif // _TIMEMEASURE_H