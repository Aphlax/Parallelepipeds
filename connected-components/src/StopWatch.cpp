/*
 * StopWatch.cpp
 *
 *  Created on: 03.12.2014
 *      Author: Seraiah
 */

#include <ctime>
#include <chrono>
#include <vector>

using namespace std;
class StopWatch {
	chrono::time_point<std::chrono::system_clock> startTime, endTime, lastCheckpointTime;
	chrono::duration<double> elapsed_seconds;
	vector<chrono::duration<double> > checkPoints;


	public: void start()
	{
		startTime = std::chrono::system_clock::now();
		lastCheckpointTime = startTime;
	}
	public: double checkPoint()
	{
		endTime = std::chrono::system_clock::now();
		elapsed_seconds = endTime-lastCheckpointTime;
		checkPoints.push_back(elapsed_seconds);
		lastCheckpointTime = endTime;
		return elapsed_seconds.count();
	}
	public: double stop()
	{
		endTime = std::chrono::system_clock::now();
		elapsed_seconds = endTime-startTime;
		return elapsed_seconds.count();
	}
};

