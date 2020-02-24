#pragma once

#include <functional>

#include "trandom_double.h"
#include "trandom_int.h"


//Метод отжига служит для поиска глобального минимума некоторой функции f(x), заданной для x из некоторого 
//пространства S дискретного или непрерывного. Элементы множества представляют собой состояния воображаемой 
//физической системы (энергетические уровни), а значение функции f в этих точках используется как энергия 
//системы E = f(x). В каждый момент предполагается заданной температура системы	T, как правило, уменьшающаяся
//с течением времени. После попадания в состояние	x при температуре T, следующее состояние системы выбирается 
//в соответствии с заданным	порождающим семейством вероятностных распределений G(x, T), которое при 
//фиксированных	x и	T задает случайный элемент x` = G(x, T) со значениями в пространстве S. После генерации
//нового состояний x` система с вероятностью h(dE, T) переходит к следующему шагу в состояние x`. В противном
//случае генерация повторяется. Здесь dE обозначает приращение функции энергии f(x) - f(x`). Величина h(dE,T)
//вероятность принятия нового состояния. Она выбирается либо как h(dE, T) = 1/(1 + exp(dE/T)) либо ее
//приближенное значение h(dE, T) = exp(-dE/T). Во втором случае вероятность оказывается больше единицы при 
//dE < 0 поэтому ее принимат за 1. Это означает, что если новое состояние лучше предыдущего, то переход в 
//него осуществляется в любом случае.	Чтобы обеспечить сходимость при заданной функции понижения температуры
//необходимо наложить ограничение на выбор последующего состояния из предыдущего.
//
//Больцмановский отжиг	(1)
//--------------------
//Изменение температуры T(k) = T0/ln(1 + k), k > 0 - номер итерации
//Семейство порождающих распределений G(x,T) выбираетс как семейство нормальных распределений с математическим 
//ожиданием x и дисперсией T:
//g(x`, x, T) = (2*PI*T)^(-D/2)	* exp(-|x` - x|^2/(2T))
//D - размерность пространства состояний
//При достаточно больших T0 и числе шагов K гарантирует нахождение глобального минимума
//Недостаток - медленное убывание температуры. Например, для того, чтобы понизить исходную температуру в 40 
//раз, требуется e^40 = 2.35*10^17 итераций.
//
//Отжиг Коши (быстрый отжиг) (2)
//----------
//Изменение температуры T(k) = T0/k, k > 0 - номер итерации
//Семейство порождающих распределений G(x,T) выбираетс как семейство распределений Коши:
//g(x`, x, T) = (1/PI)^D*T/(|x` - x|^2 + T^2)^((D+1)/2)
//D - размерность пространства состояний
//К сожалению, это распределение не очень удобно моделировать	в пространстве размерности больше 1. Этого можно 
//избежать, например, с помощью перемножения D одномерных распределений	Коши:
//g(x`, x, T) = (1/PI)^D*Пi(T/(|x`i - xi|^2 + T^2))
//но в этом случае нахождение глобального минимума гарантируется только при законе изменения температуры не 
//быстрее чем: T(k) = T0/k^(1/D) что гораздо медленнее чем в первом случае
//
//Сверхбыстрый отжиг (3)
//------------------
//Пространство S считается состоящим из	D-мерных векторов	(x1...xD) из интервала [Ai..Bi]. Кроме этого, 
//температура по каждой из координат может различаться, таким образом, T также является вектором размерности 
//D. Семейство распределений строится следующим образом. Вводится функция
//gt(y)	= Пi(1/(2*(|yi|+Ti)*ln(1+1/Ti))); yi из [-1..1]
//В качестве y для получения плотности распределения G используется dx/(Bi - Ai). Таким образом новое
//значение x`i = xi + zi*(Bi - Ai), где zi - случайная величина с плотностью g(i, T) на [-1..1]
//такую случайную величину легко промоделировать
//zi = sign(ai-1/2)*Ti*((1+1/Ti)^|2*ai-1|-1)
//где ai - независимые случайные величины	распределенные равномерно на [0..1]. Доказано, что закон изменения 
//температуры дает теоретическую гарантию нахождения глобального минимума
//Ti(k)=T(i,0)*exp(-ci*k^(1/D)), ci > 0
//Для вероятности принятия также используется отдельная шкала изменения температуры	по тому же закону.
//При реализации этого метода ci управляетя двумя параметрами
//ci = mi * exp(-ni/D)
//Преимущества такого метода очевидны. Во-первых, экспоненциальное убывание температуры гораздо быстрее 
//достижимого в	предыдущих методах. Во-вторых, разделение размерностей может дать большой выигрыш, как и 
//благодаря отдельным температурам (т. к. специфика конкретной задачи может сильно различать параметры), 
//так и благодаря ускорению процесса, в случае, если не нужно менять все координаты одновременно.
//Кроме того, в отличие от отжига Коши, сверхбыстрый отжиг, как было показано, допускает очень быстрое 
//моделирование распределения G независимо от размерности.
//Среди недостатков этого метода можно назвать большое количество параметров требующих настроики для решения 
//конкретной задачи.
//
//
//См: "Метод отжига" Лопатин А.С. 2005
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

//Как того требует схема Больцмана
//Используем нормальное распределения для получения следующего вектора состояния из предыдущего
//При этом моделируем D-мерное распределение как произведение D одномерных нормальных распределений
//При этом моделируем D-мерное распределение по каждому измерению независимо - правильно учесть нормировку
//nd * sqrt(2*PI*T) / pow(2*PI*T, D/2);
//Еще имеется вариант использовать ЦПТ и моделировать нормальное распределение как сумму 24х равномерных
inline fp GetXForBoltzmann(const fp x, const fp T, const int dim)
{
	std::random_device rd{}; //Инициализировать по всем правилам
  std::mt19937 gen{rd()};	 //
	std::normal_distribution<fp> nd(x, std::sqrt(T));
	return nd(gen) * std::sqrt(2*std::_Pi*T) / std::pow(2*std::_Pi*T, dim/2);
}

//Как того требует схема сверхбыстрого отжига
//Моделируем распределение методом обратных функций
//При этом моделируем D-мерное распределение по каждому измерению независимо
inline fp GetXForVeryFastAnnealing(const fp x, const fp T)
{
	fp a = TRandomDouble::Instance()();
	return sign(a - fp(1)/2) * T * (pow(1 + fp(1)/T, abs(2*a - 1)) - 1);
}

//Нужно чтоб при tmin P->1 для того de не сильно большой(нормировать на 1, тогда и t можно начинать с 1)
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
					//!!!Можно свернуть эту часть условия, задав правильным образом TransitionProbability
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