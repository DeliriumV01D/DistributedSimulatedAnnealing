#include <QApplication>

#include "distributed_simulated_annealing.h"
#include "window.h"
#include "travelling_salesman.h"


int main(int argc, char *argv[])
{
	int rank = 0;
	TETSP * etsp;
	SimulatedAnnealing::TDistributedSimulatedAnnealing<TETSPState> * annealing;

	etsp = new TETSP(100);
	annealing = new SimulatedAnnealing::TDistributedSimulatedAnnealing<TETSPState>(
		etsp,																										//just an example of the state generator - travelling salesman problem state generator 
		SimulatedAnnealing::BoltzmannTransitionProbability,			//set transition probability
		SimulatedAnnealing::DecreaseLinear,											//set temperature decrease function 
		SimulatedAnnealing::DecreaseLinearDefaults,							//set simulating annealing params
		25,																											//set number of distributed iterations for annealing process 
		8																												//set limit on the input queue
	);

	rank = annealing->GetMPIRank();

	//Q_INIT_RESOURCE(basicdrawing);
	if (rank == 0)	 //user interface is created only in process #0
	{
    QApplication app(argc, argv);
    Window window(annealing);
    window.show();
    return app.exec();
	} else {				//other processes make calculations
		(*annealing)();
	}

	delete annealing;
	delete etsp;
}				 