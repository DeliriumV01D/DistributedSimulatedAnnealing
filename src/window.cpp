#include "window.h"

#include <QPainter>
#include <QtWidgets>

Window :: Window(SimulatedAnnealing::TDistributedSimulatedAnnealing<TETSPState> * annealing)
{
	Annealing = annealing;
	btnStart = new QPushButton("Start", this);
	connect(btnStart, SIGNAL(clicked()), this, SLOT(OnBtnStartClicked()));
}

Window :: ~Window()
{

}

void Window :: paintEvent(QPaintEvent * /* event */)
{
	QPainter painter(this);

	TETSPState * state = Annealing->GetCurrentState();

	for (auto it = state->Sequence.begin(); it != state->Sequence.end(); it++)
	{	
		auto it1 = it;
		auto it2 = it;
		if (it != state->Sequence.begin())
			it2 = std::prev(it);
		else 
			it2 = std::prev(state->Sequence.end()); 

		painter.drawLine(
			QPoint(state->Points[*it1][0] * width(), state->Points[*it1][1] * height()), 
			QPoint(state->Points[*(it2)][0] * width(), state->Points[*(it2)][1] * height())
		);
		painter.drawEllipse(state->Points[*it][0] * width() - 5, state->Points[*it][1] * height() - 5, 10, 10);
	}

	painter.setRenderHint(QPainter::Antialiasing, false);
	painter.setPen(palette().dark().color());
	painter.setBrush(Qt::NoBrush);
	painter.drawRect(QRect(0, 0, width() - 1, height() - 1));
}

void Window :: OnBtnStartClicked()
{
	(*Annealing)();
}