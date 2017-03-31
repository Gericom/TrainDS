#ifndef __OS_VALARM_H__
#define __OS_VALARM_H__

namespace OS
{
	class VAlarm
	{
	public:
		OSVAlarm mOSVAlarm;

		VAlarm()
		{
			OS_CreateVAlarm(&mOSVAlarm);
		}

		~VAlarm()
		{
			Cancel();
		}

		void Set(s16 count, s16 delay, OSVAlarmHandler handler, void* arg)
		{
			OS_SetVAlarm(&mOSVAlarm, count, delay, handler, arg);
		}

		void SetPeriodic(s16 count, s16 delay, OSVAlarmHandler handler, void* arg)
		{
			OS_SetPeriodicVAlarm(&mOSVAlarm, count, delay, handler, arg);
		}

		void Cancel()
		{
			OS_CancelVAlarm(&mOSVAlarm);
		}
	};
}

#endif