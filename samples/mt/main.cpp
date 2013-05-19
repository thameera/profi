﻿#include "precompiled.h"
#include <fstream>

#define SIMULATE_WORK
#ifdef SIMULATE_WORK
	#define SIMUL_SLEEP(TIME) std::this_thread::sleep_for(std::chrono::milliseconds(TIME));
#else
	#define SIMUL_SLEEP(TIME)
#endif

void foo() {
	PROFI_FUNC
	SIMUL_SLEEP(500)
}

void recurse(unsigned times)
{
	PROFI_FUNC
	SIMUL_SLEEP(100)

	if(times == 0)
		return;
	recurse(--times);
}

std::atomic<bool> threadStart = false;

void thread_run1() {
	// wait for all threads to start
	while(!threadStart);

	PROFI_SCOPE("ThreadScope1")
	foo();
	SIMUL_SLEEP(200)
}

void thread_run2() {
	// wait for all threads to start
	while(!threadStart);

	PROFI_FUNC
	SIMUL_SLEEP(500)
	recurse(2);
}

int main()
{
	profi::DefaultAllocator allocator;
	profi::Initialize(&allocator);
	{
		PROFI_FUNC
		{
			PROFI_SCOPE("StartThreads")
			std::thread t1(&thread_run1);
			std::thread t2(&thread_run2);

			threadStart = true;

			t1.join();
			t2.join();
		}
	}

	// dump the report
	profi::IReport* report(profi::GetReportJSON());
	
	std::ofstream fout("output.json");
	fout.write((const char*)report->Data(), report->Size());

	report->Release();
	profi::Deinitialize();
}