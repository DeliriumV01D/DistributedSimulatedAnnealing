#pragma once

#include <iostream>
#include <array>

#include "simulated_annealing.h"

const int DIM = 2;

///Point
typedef std::array<float, DIM> TPoint;

///Distance between two points
inline float Distance(const TPoint &p1, const TPoint &p2)
{
	float result = 0;
	for (int i = 0; i < DIM; i++)
		result += std::pow(p1[i] - p2[i], 2.f);
	result = std::sqrt(result);
	return result;
}

struct TETSPState {
	//array of points
	std::vector<TPoint> Points;
	//point traversal order
	std::vector<unsigned long> Sequence;
	//
	std::vector<unsigned char> Buffer;

	size_t GetBufferSize()
	{
		//Size(points number) + size * DIM * sizeof(float) + size * sizeof(unsigned long)
		return sizeof(size_t) + Points.size() * DIM * sizeof(float) + Sequence.size() * sizeof(unsigned long);
	}

	unsigned char * ToBuffer()
	{
		Buffer.resize(GetBufferSize());
		size_t sz = Points.size();
		memcpy(Buffer.data(), &sz, sizeof(sz));
		int pos = sizeof(sz);
		sz = sizeof(float) * DIM;
		for (int i = 0; i < Points.size(); i++)
		{
			memcpy(Buffer.data() + pos, &Points[i][0], sz);
			pos += (int)sz;
		}
		memcpy(Buffer.data() + pos, Sequence.data(), Sequence.size() * sizeof(unsigned long));
		return Buffer.data();
	}

	void FromBuffer(unsigned char * buffer)
	{
		size_t sz;
		int pos;
		memcpy(&sz, buffer, sizeof(sz));
		Points.resize(sz);
		Sequence.resize(sz);
		pos = sizeof(sz);
		sz = sizeof(float) * DIM;
		for (int i = 0; i < Points.size(); i++)
		{
			memcpy(&Points[i][0], buffer + pos, sz);
			pos += (int)sz;
		}
		memcpy(Sequence.data(), buffer + pos, Sequence.size() * sizeof(unsigned long));
	};
};

//Euclidian travelling salesman problem
class TETSP	: public SimulatedAnnealing::IStateGenerator<TETSPState> {
protected:
	int PointCount;

	float Energy(const std::vector<TPoint> &points, const std::vector<unsigned long> &sequence)	
	{
		float result = 0;
		for (int i = 0; i < sequence.size(); i++)
			result += Distance(points[sequence[i]], points[sequence[(i == sequence.size() - 1) ? 0 : (i+1)]]);

		return result;
	}
public:
	TETSP(const int point_count)
	{
		PointCount = point_count;
	}

	//New state generation function based on the current state
	TETSPState GenerateState(const TETSPState &state, bool reinitialize = false) override
	{
		TETSPState result;
		result.Points.resize(PointCount);
		result.Sequence.resize(result.Points.size());

		//initialization
		if (state.Points.size() == 0 || reinitialize)
		{
			for (auto &it : result.Points)
				for (int j = 0; j < DIM; j++)
					it[j] = (float)TRandomDouble::Instance()();

			for (unsigned long i = 0; i < result.Sequence.size(); i++)
				result.Sequence[i] = i;
		}	else {
			//First, copy all the elements of the initial state and its traversal order
			std::copy(state.Points.begin(), state.Points.end(), result.Points.begin());
			std::copy(state.Sequence.begin(), state.Sequence.end(), result.Sequence.begin());
			//Now randomly swap the order of traversal of a pair of elements
			unsigned	temp,
								pos1 = TRandomInt::Instance()()	% (result.Sequence.size()+1),
								pos2 = TRandomInt::Instance()()	% (result.Sequence.size()+1);
			if (pos1 > pos2)
			{
				temp = pos1;
				pos1 = pos2;
				pos2 = temp;
			}
			std::reverse_copy(state.Sequence.begin() + pos1, state.Sequence.begin() + pos2, result.Sequence.begin() + pos1);
		}
		return result;
	}

	SimulatedAnnealing::fp Energy(const TETSPState &state) override
	{
		return Energy(state.Points, state.Sequence);
	}
};					//TETSP