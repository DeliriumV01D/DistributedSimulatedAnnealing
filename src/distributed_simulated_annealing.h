#pragma once

#include "simulated_annealing.h"
#include "../MPIProcessPool/src/tmpi_process_pool.h"


namespace SimulatedAnnealing {

///At this moment the task is characterized only by its identifier
class TSimulatedAnnealingTask : public IMPITask {
protected:
	int ID;
public:
	TSimulatedAnnealingTask(const int id = 0) : IMPITask()
	{
		ID = id;
	}

	int GetID()
	{
		return ID;
	}

	size_t GetBufferSize()
	{
		return sizeof(int);
	}

	unsigned char * ToBuffer()
	{
		return (unsigned char *)&ID;
	}

	void FromBuffer(unsigned char * buffer)
	{
		ID = *(int *)buffer;
	};
};

///Task identifier	+ found state and its energy
template <typename TState, typename TEnergy>
class TSimulatedAnnealingResult : public IMPIResult {
	int ID;
	TState State;
	TEnergy Energy;
	std::vector<unsigned char> Buffer;
public:
	TSimulatedAnnealingResult(
		const TState &state,
		const TEnergy &energy,
		const int id = 0
	)	: IMPIResult()
	{
		State = state;
		Energy = energy;
		ID = id;
	}

	TSimulatedAnnealingResult() : IMPIResult()
	{};
		
	int GetID()
	{
		return ID;
	}

	TState * GetState()
	{ 
		return &State;
	}
	
	TEnergy GetEnergy()
	{
		return Energy;
	}

	size_t GetBufferSize()
	{
		return sizeof(int) + State.GetBufferSize() + sizeof(Energy);
	}

	unsigned char * ToBuffer()
	{
		Buffer.resize(GetBufferSize());
		memcpy(Buffer.data(), &ID, sizeof(ID));
		int pos = sizeof(ID);
		memcpy(Buffer.data() + pos, &Energy, sizeof(Energy));
		pos += sizeof(Energy);
		unsigned char * temp = State.ToBuffer();
		memcpy(Buffer.data() + pos, temp, State.GetBufferSize());
		return Buffer.data();
	}

	void FromBuffer(unsigned char * buffer)
	{
		ID = *(int *)buffer;
		Energy = *(TEnergy *)(buffer + sizeof(ID));
		State.FromBuffer(buffer + sizeof(ID) + sizeof(Energy));
	};
};


///
template <typename TState>
class TSimulatedAnnealingFunc {
protected:
	TSimulatedAnnealing<TState> * SimulatedAnnealing;

	TSimulatedAnnealingFunc(const TSimulatedAnnealingFunc &sa_func)
	{}
public:
	TSimulatedAnnealingFunc()
	{
		SimulatedAnnealing = nullptr;
	}

	~TSimulatedAnnealingFunc()
	{
		if (SimulatedAnnealing != nullptr)
			delete SimulatedAnnealing;
	}

	void InitSimulatedAnnealing(
		IStateGenerator<TState> * state_generator,
		std::function<fp (const fp &de, const fp &t)> transition_probability,
		std::function<fp (double &T0, const fp &decrease_score, const int64_t k)> decrease_temperature,
		const TSimulatedAnnealingParams &params
	){
		SimulatedAnnealing = new SimulatedAnnealing::TSimulatedAnnealing<TState>(
			state_generator, 
			transition_probability, 
			decrease_temperature, 
			params
		);
	}

	//std::function<TResult (TTask*)> result_func
	TSimulatedAnnealingResult<TState, fp> operator ()(TSimulatedAnnealingTask * task)
	{
		//Reinitialize CurrentState
		SimulatedAnnealing->Reinitialize();
		//Optimize
		(*SimulatedAnnealing)();
		//Return result
		TSimulatedAnnealingResult<TState, fp> result(		
			*SimulatedAnnealing->GetCurrentState(),
			SimulatedAnnealing->GetCurrentEnergy(),
			task->GetID()
		);
		return result;
	}
};

///
template <typename TState>
class TDistributedSimulatedAnnealing {
protected:
	TState CurrentState;
	fp CurrentEnergy;
	TSimulatedAnnealingFunc<TState> * SAFunc;
	TMPIProcessPool <TSimulatedAnnealingTask, TSimulatedAnnealingResult<TState, fp>> * MPIProcessPool;
	int MPIRank,
			Iterations;
public:
	TDistributedSimulatedAnnealing(
		IStateGenerator<TState> * state_generator,
		std::function<fp (const fp &de, const fp &t)> transition_probability /*= BoltzmannTransitionProbability*/,
		std::function<fp (double &T0, const fp &decrease_score, const int64_t k)> decrease_temperature /*= DecreaseLinear*/,
		const TSimulatedAnnealingParams &params /*= DecreaseLinearDefaults*/,
		const int iterations,
		const int task_queue_max_length
	){
		Iterations = iterations;
		//Create functor, incapsulating TSimulatedAnnealing, that gets task object and returns result object
		//	but initialize it only when seed will be received
		SAFunc = new TSimulatedAnnealingFunc<TState>();
		
		MPIProcessPool = new TMPIProcessPool <TSimulatedAnnealingTask, TSimulatedAnnealingResult<TState, fp>>(std::ref(*SAFunc), task_queue_max_length);
		MPIRank = MPIProcessPool->GetMPIRank();
		unsigned long seed = time(0) + (/*+thread.getId()*18181 + thread.getId()*/ + MPIRank*17471 + MPIRank)%(16661);

		if (MPIRank == 0) {
			//ready to distribute tasks and output results
		} else {
			//ready to calculate
			//seed passed by parameter in SimulatedAnnealing
			TSimulatedAnnealingParams p = params;
			p.seed	= seed;
			SAFunc->InitSimulatedAnnealing(
				state_generator,
				transition_probability,
				decrease_temperature,
				p
			);
		}
	}

	~TDistributedSimulatedAnnealing()
	{
		delete MPIProcessPool;
		delete SAFunc;
	}

	void operator ()()
	{
		bool first_result = true;
		//this function is needed to parse the result queue
		auto get_results = [this, &first_result]()
		{
			IMPIResult ** result = new IMPIResult *;
			while (MPIProcessPool->GetResult(result))
			{
				std::cout<<"result: "<<((TSimulatedAnnealingResult<TState, fp>*)(*result))->GetID()<<std::endl;
				if (first_result || ((TSimulatedAnnealingResult<TState, fp>*)(*result))->GetEnergy() < CurrentEnergy)
				{
					CurrentState = *((TSimulatedAnnealingResult<TState, fp>*)(*result))->GetState();
					CurrentEnergy = ((TSimulatedAnnealingResult<TState, fp>*)(*result))->GetEnergy();
					first_result = false;
					std::cout	<<"solution improved: "<<((TSimulatedAnnealingResult<TState, fp>*)(*result))->GetID()<<" "<<
										((TSimulatedAnnealingResult<TState, fp>*)(*result))->GetEnergy()<<std::endl;
				}
				delete (*result);
			}
			delete result;
		};
	
		MPIProcessPool->Start();
	
		if (MPIProcessPool->GetMPIRank() == 0)
		{
			int i = 0;
			//task are distributed and the results queue is parsed
			do {
				if (MPIProcessPool->AddTask(new TSimulatedAnnealingTask(i)))
				{
					i++;
				} else {
					get_results();
					std::this_thread::sleep_for(std::chrono::milliseconds(1));
				}
			} while (i < Iterations);

			//wait for the end of the calculations and take the remaining results
			while (!MPIProcessPool->IsFinished())
			{
				std::this_thread::sleep_for(std::chrono::milliseconds(1));
				get_results();
			}
		}
		//process #0 will stop when all answers are received, other processes will stop by command
		//mpi_process_pool.Stop();	
	}

	TState * GetCurrentState()
	{
		return &CurrentState;
	}

	fp GetCurrentEnergy()
	{
		return CurrentEnergy;
	}

	int GetMPIRank()
	{
		return MPIRank;
	}
};

};	//namespace SimulatedAnnealing