/*
 * StopWatch.cpp
 *
 *  Created on: 03.12.2014
 *      Author: Seraiah
 */
#ifndef Included_STOPWATCH
#define Included_STOPWATCH

#include <chrono>
#include <vector>

using namespace std;
class StopWatch {
	public: enum Section {inputProcessing, mainSection, merging};
	chrono::time_point<chrono::system_clock> startTime, endTime, lastCheckpointTime;
	chrono::duration<double> elapsed_seconds;
	//vector<chrono::duration<double> > checkPoints;
	double inputProcessingTime = 0;
	double mainSectionTime = 0;
	double mergingTime = 0;

	public: void start(Section e)
	{
		startTime = std::chrono::system_clock::now();
	}
	public: double stop(Section e)
	{
		endTime = std::chrono::system_clock::now();
		elapsed_seconds = endTime-startTime;
		double time = elapsed_seconds.count();

		if(e==inputProcessing)
			inputProcessingTime = time;
		else if(e==mainSection)
			mainSectionTime = time;
		else
			mergingTime = time;
		return time;
	}
};


#endif
