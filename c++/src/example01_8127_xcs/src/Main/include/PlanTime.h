
#ifndef __PLAN_TIME_H_
#define __PLAN_TIME_H_

#include <assert.h>
#include <semaphore.h>
#include "lstLib.h"

#ifndef ASSERT
#define ASSERT(x)	assert(x)
#endif

typedef struct tagTimeSegmentNode
{
	NODE node;
	unsigned int utcStart;
	unsigned int utcEnd;
}TSN;

class CPlanTime
{
	public:
		CPlanTime();
		virtual ~CPlanTime();

        virtual int ShowPlanTime() = 0;
        virtual int IsInPlanTime(unsigned int utc) = 0;
        virtual int EmptyPlanTime() = 0;
        virtual CPlanTime* clone() const = 0;
		/*virtual int GetValidPlanTimeSlice(unsigned int &utcStart,unsigned int &utcEnd);*/

	protected:
		
};

class COncePlanTime :public CPlanTime
{
	public:
		COncePlanTime();
		~COncePlanTime();

		int LoadPlanTime(unsigned int utcStart,unsigned int utcEnd);

		int ShowPlanTime();
		int IsInPlanTime(unsigned int utc);
		int EmptyPlanTime();
        virtual COncePlanTime* clone() const  \
        { return new COncePlanTime(*this); }
		/*int GetValidPlanTimeSlice(unsigned int &utcStart,unsigned int &utcEnd);*/

	private:
		LIST m_lstOncePlan;
};

class CDayPlanTime :public CPlanTime
{
	public:
		CDayPlanTime();
		~CDayPlanTime();

		int LoadPlanTime(unsigned int utcDayStart,unsigned int utcDayEnd);
		
		int ShowPlanTime();
		int IsInPlanTime(unsigned int utc);
		int EmptyPlanTime();
        virtual CDayPlanTime* clone()  const \
        { return new CDayPlanTime(*this); }
		/*int GetValidPlanTimeSlice(unsigned int &utcStart,unsigned int &utcEnd);*/

	private:
		LIST m_lstDayPlan;		
};

class CWeekPlanTime :public CPlanTime
{
	public:
		CWeekPlanTime();
		~CWeekPlanTime();

		int LoadPlanTime(int nWeekDay,unsigned int utcDayStart,unsigned int utcDayEnd);
		
		int ShowPlanTime();
		int IsInPlanTime(unsigned int utc);
		int EmptyPlanTime();
        virtual CWeekPlanTime* clone()  const \
        { return new CWeekPlanTime(*this); }
		/*int GetValidPlanTimeSlice(unsigned int &utcStart,unsigned int &utcEnd);*/

	private:
		LIST m_lstWeekPlan[7];
};

#endif

