#pragma once

#include <QWidget>

#include "travelling_salesman.h"
#include "distributed_simulated_annealing.h"


QT_BEGIN_NAMESPACE
class QPushButton;
QT_END_NAMESPACE

///Minimalistic Qt UI - "start" button and painter which draws a path right on backgroung of the window
class Window : public QWidget {
	Q_OBJECT
protected:
	QPushButton * btnStart;
  void paintEvent(QPaintEvent *event) override;
	SimulatedAnnealing::TDistributedSimulatedAnnealing<TETSPState> * Annealing;
public:
	Window(SimulatedAnnealing::TDistributedSimulatedAnnealing<TETSPState> * annealing);
	~Window();
public slots:
	void OnBtnStartClicked();
};