#pragma once

#include <functional>

#include "trandom_double.h"
#include "trandom_int.h"


//����� ������ ������ ��� ������ ����������� �������� ��������� ������� f(x), �������� ��� x �� ���������� 
//������������ S ����������� ��� ������������. �������� ��������� ������������ ����� ��������� ������������ 
//���������� ������� (�������������� ������), � �������� ������� f � ���� ������ ������������ ��� ������� 
//������� E = f(x). � ������ ������ �������������� �������� ����������� �������	T, ��� �������, �������������
//� �������� �������. ����� ��������� � ���������	x ��� ����������� T, ��������� ��������� ������� ���������� 
//� ������������ � ��������	����������� ���������� ������������� ������������� G(x, T), ������� ��� 
//�������������	x �	T ������ ��������� ������� x` = G(x, T) �� ���������� � ������������ S. ����� ���������
//������ ��������� x` ������� � ������������ h(dE, T) ��������� � ���������� ���� � ��������� x`. � ���������
//������ ��������� �����������. ����� dE ���������� ���������� ������� ������� f(x) - f(x`). �������� h(dE,T)
//����������� �������� ������ ���������. ��� ���������� ���� ��� h(dE, T) = 1/(1 + exp(dE/T)) ���� ��
//������������ �������� h(dE, T) = exp(-dE/T). �� ������ ������ ����������� ����������� ������ ������� ��� 
//dE < 0 ������� �� �������� �� 1. ��� ��������, ��� ���� ����� ��������� ����� �����������, �� ������� � 
//���� �������������� � ����� ������.	����� ���������� ���������� ��� �������� ������� ��������� �����������
//���������� �������� ����������� �� ����� ������������ ��������� �� �����������.
//
//�������������� �����	(1)
//--------------------
//��������� ����������� T(k) = T0/ln(1 + k), k > 0 - ����� ��������
//��������� ����������� ������������� G(x,T) ��������� ��� ��������� ���������� ������������� � �������������� 
//��������� x � ���������� T:
//g(x`, x, T) = (2*PI*T)^(-D/2)	* exp(-|x` - x|^2/(2T))
//D - ����������� ������������ ���������
//��� ���������� ������� T0 � ����� ����� K ����������� ���������� ����������� ��������
//���������� - ��������� �������� �����������. ��������, ��� ����, ����� �������� �������� ����������� � 40 
//���, ��������� e^40 = 2.35*10^17 ��������.
//
//����� ���� (������� �����) (2)
//----------
//��������� ����������� T(k) = T0/k, k > 0 - ����� ��������
//��������� ����������� ������������� G(x,T) ��������� ��� ��������� ������������� ����:
//g(x`, x, T) = (1/PI)^D*T/(|x` - x|^2 + T^2)^((D+1)/2)
//D - ����������� ������������ ���������
//� ���������, ��� ������������� �� ����� ������ ������������	� ������������ ����������� ������ 1. ����� ����� 
//��������, ��������, � ������� ������������ D ���������� �������������	����:
//g(x`, x, T) = (1/PI)^D*�i(T/(|x`i - xi|^2 + T^2))
//�� � ���� ������ ���������� ����������� �������� ������������� ������ ��� ������ ��������� ����������� �� 
//������� ���: T(k) = T0/k^(1/D) ��� ������� ��������� ��� � ������ ������
//
//������������ ����� (3)
//------------------
//������������ S ��������� ��������� ��	D-������ ��������	(x1...xD) �� ��������� [Ai..Bi]. ����� �����, 
//����������� �� ������ �� ��������� ����� �����������, ����� �������, T ����� �������� �������� ����������� 
//D. ��������� ������������� �������� ��������� �������. �������� �������
//gt(y)	= �i(1/(2*(|yi|+Ti)*ln(1+1/Ti))); yi �� [-1..1]
//� �������� y ��� ��������� ��������� ������������� G ������������ dx/(Bi - Ai). ����� ������� �����
//�������� x`i = xi + zi*(Bi - Ai), ��� zi - ��������� �������� � ���������� g(i, T) �� [-1..1]
//����� ��������� �������� ����� ���������������
//zi = sign(ai-1/2)*Ti*((1+1/Ti)^|2*ai-1|-1)
//��� ai - ����������� ��������� ��������	�������������� ���������� �� [0..1]. ��������, ��� ����� ��������� 
//����������� ���� ������������� �������� ���������� ����������� ��������
//Ti(k)=T(i,0)*exp(-ci*k^(1/D)), ci > 0
//��� ����������� �������� ����� ������������ ��������� ����� ��������� �����������	�� ���� �� ������.
//��� ���������� ����� ������ ci ���������� ����� �����������
//ci = mi * exp(-ni/D)
//������������ ������ ������ ��������. ��-������, ���������������� �������� ����������� ������� ������� 
//����������� �	���������� �������. ��-������, ���������� ������������ ����� ���� ������� �������, ��� � 
//��������� ��������� ������������ (�. �. ��������� ���������� ������ ����� ������ ��������� ���������), 
//��� � ��������� ��������� ��������, � ������, ���� �� ����� ������ ��� ���������� ������������.
//����� ����, � ������� �� ������ ����, ������������ �����, ��� ���� ��������, ��������� ����� ������� 
//������������� ������������� G ���������� �� �����������.
//����� ����������� ����� ������ ����� ������� ������� ���������� ���������� ��������� ��������� ��� ������� 
//���������� ������.
//
//
//��: "����� ������" ������� �.�. 2005
//
//(1)Metropolis N., Rosenbluth	A. W., Rosenbluth	M. N., Teller A. H., and Teller E.
//Equation of State Calculations by Fast Computer Machines // J. Chemical Physics. 21. 6. June.	1953. P. 1087-1092.
//
//(2)Szu H. H., Hartley R. L. Fast Simulated Annealing //Physical Letters A. 122. 1987. P. 157-162.
//
//(3)Ingber L. Very fast simulated re-annealing // Mathematical and Computer Modelling. 12. 1989. P. 967973.

namespace SimulatedAnnealing {

typedef double fp;

template <typename T> int sign(T val) 
{
	return (T(0) < val) - (val < T(0));
}

struct TSimulatedAnnealingParams {
	fp TempDecreaseScore;
	fp Tbegin;
	fp Tend;
	unsigned int	seed,
								dim;
};

const TSimulatedAnnealingParams DecreaseLinearDefaults {
	0.001,
	1.e-0,
	1.e-4,
	0,
	2
};

const TSimulatedAnnealingParams DecreaseLogarithmicDefaults {
	5,
	1.e-0,
	1.e-3,
	0,
	2
};

inline fp DecreaseLinear(const fp &T0, const fp &decrease_score, const int64_t k)
{
	if (k == 0)
		return T0;
	return T0/k/decrease_score;
};

inline fp DecreaseLogarithmic(const fp &T0, const fp &decrease_score, const int64_t k)
{
	return T0/log(1 + k)/decrease_score;
};

//��� ���� ������� ����� ���������
//���������� ���������� ������������� ��� ��������� ���������� ������� ��������� �� �����������
//��� ���� ���������� D-������ ������������� ��� ������������ D ���������� ���������� �������������
//��� ���� ���������� D-������ ������������� �� ������� ��������� ���������� - ��������� ������ ����������
//nd * sqrt(2*PI*T) / pow(2*PI*T, D/2);
//��� ������� ������� ������������ ��� � ������������ ���������� ������������� ��� ����� 24� �����������
inline fp GetXForBoltzmann(const fp x, const fp T, const int dim)
{
	std::random_device rd{}; //���������������� �� ���� ��������
  std::mt19937 gen{rd()};	 //
	std::normal_distribution<fp> nd(x, std::sqrt(T));
	return nd(gen) * std::sqrt(2*std::_Pi*T) / std::pow(2*std::_Pi*T, dim/2);
}

//��� ���� ������� ����� ������������� ������
//���������� ������������� ������� �������� �������
//��� ���� ���������� D-������ ������������� �� ������� ��������� ����������
inline fp GetXForVeryFastAnnealing(const fp x, const fp T)
{
	fp a = TRandomDouble::Instance()();
	return sign(a - fp(1)/2) * T * (pow(1 + fp(1)/T, abs(2*a - 1)) - 1);
}

//����� ���� ��� tmin P->1 ��� ���� de �� ������ �������(����������� �� 1, ����� � t ����� �������� � 1)
inline fp BoltzmannTransitionProbability(const fp &de, const fp &t)
{																															 
	return exp(-de/t);
}

//Base interface class for all generators and energies
template <typename TState>
class IStateGenerator {
public:
	~IStateGenerator(){};
	virtual TState GenerateState(const TState &state, bool reinitialize = false){return TState();};
	//GenerateStateBuffer() - for the network package 
	virtual fp Energy(const TState &state){return 0;};
};

//
template <typename TState>
class TSimulatedAnnealing {
protected:
	TSimulatedAnnealingParams Params;
	IStateGenerator<TState> * StateGenerator;
	std::function<fp (double &T0, const fp &decrease_score, const int64_t k)> DecreaseTemperature;
	std::function<fp (const fp &de, const fp &t)> TransitionProbability;
	fp T;
	TState CurrentState;
	fp CurrentEnergy;
public:
	TSimulatedAnnealing(
		IStateGenerator<TState> * state_generator,
		std::function<fp (const fp &de, const fp &t)> transition_probability = BoltzmannTransitionProbability,
		std::function<fp (double &T0, const fp &decrease_score, const int64_t k)> decrease_temperature = DecreaseLinear,
		const TSimulatedAnnealingParams &params = DecreaseLinearDefaults
	)
	{
		TRandomInt::Instance().Initialize(params.seed);
		TRandomDouble::Instance().Initialize(TRandomInt::Instance()());
		StateGenerator = state_generator;
		TransitionProbability = transition_probability;
		DecreaseTemperature = decrease_temperature;
		Params = params;	

		//Initialization
		Reinitialize();
	}

	//Initialization
	void Reinitialize()
	{
		CurrentState = StateGenerator->GenerateState(CurrentState, true);
		CurrentEnergy = StateGenerator->Energy(CurrentState);				//Energy of the initial state
	}

	void operator ()()
	{
		T = Params.Tbegin;																					//Start temperature is determined by the parameter value
		TState new_state = CurrentState;
		fp	new_energy = CurrentEnergy,
				temp_min_energy = 0;
		int64_t i = 0;
		while (T > Params.Tend)
		{
				new_state = StateGenerator->GenerateState(CurrentState);
				new_energy = StateGenerator->Energy(new_state);	
				if (new_energy < CurrentEnergy) {
					//!!!����� �������� ��� ����� �������, ����� ���������� ������� TransitionProbability
					CurrentState = new_state;
					CurrentEnergy = new_energy;
				} else {
					fp transition_probability = TransitionProbability(new_energy - CurrentEnergy, T);
					if (TRandomDouble::Instance()() <= transition_probability)
					{
						CurrentState = new_state;
						CurrentEnergy = new_energy;
					}
				}
			T = DecreaseTemperature(Params.Tbegin, Params.TempDecreaseScore, i);
			i++;
		}						//while (T > Params.Tmin)
	}

	TState * GetCurrentState()
	{
		return &CurrentState;
	}

	fp GetCurrentEnergy()
	{
		return CurrentEnergy;
	}
};

};	//namespace SimulatedAnnealing