The program implements distributed simulated annealing algorithm.
The simulated annealing method is a universal and general method with a small number of restrictions that serves to search for the global minimum of some function.
For example, the travelling salesman problem is solved.
Everything that relates to the method is contained in the files distributed_simulated_annealing.h and simulated_annealing.h
Other files are needed for a example of use. This project uses MPIProcessPool(https://github.com/DeliriumV01D/MPI-process-pool) project and depends on MPI library.
All kinds of cooperation are welcome.

Метод отжига служит для поиска глобального минимума некоторой функции f(x), заданной для x из некоторого 
пространства S дискретного или непрерывного. Элементы множества представляют собой состояния воображаемой 
физической системы (энергетические уровни), а значение функции f в этих точках используется как энергия 
системы E = f(x). В каждый момент предполагается заданной температура системы	T, как правило, уменьшающаяся
с течением времени. После попадания в состояние	x при температуре T, следующее состояние системы выбирается 
в соответствии с заданным	порождающим семейством вероятностных распределений G(x, T), которое при 
фиксированных	x и	T задает случайный элемент x` = G(x, T) со значениями в пространстве S. После генерации
нового состояний x` система с вероятностью h(dE, T) переходит к следующему шагу в состояние x`. В противном
случае генерация повторяется. Здесь dE обозначает приращение функции энергии f(x) - f(x`). Величина h(dE,T)
вероятность принятия нового состояния. Она выбирается либо как h(dE, T) = 1/(1 + exp(dE/T)) либо ее
приближенное значение h(dE, T) = exp(-dE/T). Во втором случае вероятность оказывается больше единицы при 
dE < 0 поэтому ее принимат за 1. Это означает, что если новое состояние лучше предыдущего, то переход в 
него осуществляется в любом случае.	Чтобы обеспечить сходимость при заданной функции понижения температуры
необходимо наложить ограничение на выбор последующего состояния из предыдущего.

Больцмановский отжиг	(1)
--------------------
Изменение температуры T(k) = T0/ln(1 + k), k > 0 - номер итерации
Семейство порождающих распределений G(x,T) выбираетс как семейство нормальных распределений с математическим 
ожиданием x и дисперсией T:
g(x`, x, T) = (2*PI*T)^(-D/2)	* exp(-|x` - x|^2/(2T))
D - размерность пространства состояний
При достаточно больших T0 и числе шагов K гарантирует нахождение глобального минимума
Недостаток - медленное убывание температуры. Например, для того, чтобы понизить исходную температуру в 40 
раз, требуется e^40 = 2.35*10^17 итераций.

Отжиг Коши (быстрый отжиг) (2)
----------
Изменение температуры T(k) = T0/k, k > 0 - номер итерации
Семейство порождающих распределений G(x,T) выбираетс как семейство распределений Коши:
g(x`, x, T) = (1/PI)^D*T/(|x` - x|^2 + T^2)^((D+1)/2)
D - размерность пространства состояний
К сожалению, это распределение не очень удобно моделировать	в пространстве размерности больше 1. Этого можно 
избежать, например, с помощью перемножения D одномерных распределений	Коши:
g(x`, x, T) = (1/PI)^D*Пi(T/(|x`i - xi|^2 + T^2))
но в этом случае нахождение глобального минимума гарантируется только при законе изменения температуры не 
быстрее чем: T(k) = T0/k^(1/D) что гораздо медленнее чем в первом случае

Сверхбыстрый отжиг (3)
------------------
Пространство S считается состоящим из	D-мерных векторов	(x1...xD) из интервала [Ai..Bi]. Кроме этого, 
температура по каждой из координат может различаться, таким образом, T также является вектором размерности 
D. Семейство распределений строится следующим образом. Вводится функция
gt(y)	= Пi(1/(2*(|yi|+Ti)*ln(1+1/Ti))); yi из [-1..1]
В качестве y для получения плотности распределения G используется dx/(Bi - Ai). Таким образом новое
значение x`i = xi + zi*(Bi - Ai), где zi - случайная величина с плотностью g(i, T) на [-1..1]
такую случайную величину легко промоделировать
zi = sign(ai-1/2)*Ti*((1+1/Ti)^|2*ai-1|-1)
где ai - независимые случайные величины	распределенные равномерно на [0..1]. Доказано, что закон изменения 
температуры дает теоретическую гарантию нахождения глобального минимума
Ti(k)=T(i,0)*exp(-ci*k^(1/D)), ci > 0
Для вероятности принятия также используется отдельная шкала изменения температуры	по тому же закону.
При реализации этого метода ci управляетя двумя параметрами
ci = mi * exp(-ni/D)
Преимущества такого метода очевидны. Во-первых, экспоненциальное убывание температуры гораздо быстрее 
достижимого в	предыдущих методах. Во-вторых, разделение размерностей может дать большой выигрыш, как и 
благодаря отдельным температурам (т. к. специфика конкретной задачи может сильно различать параметры), 
так и благодаря ускорению процесса, в случае, если не нужно менять все координаты одновременно.
Кроме того, в отличие от отжига Коши, сверхбыстрый отжиг, как было показано, допускает очень быстрое 
моделирование распределения G независимо от размерности.
Среди недостатков этого метода можно назвать большое количество параметров требующих настроики для решения 
конкретной задачи.


См: "Метод отжига" Лопатин А.С. 2005

(1)Metropolis N., Rosenbluth	A. W., Rosenbluth	M. N., Teller A. H., and Teller E.
Equation of State Calculations by Fast Computer Machines // J. Chemical Physics. 21. 6. June.	1953. P. 1087-1092.

(2)Szu H. H., Hartley R. L. Fast Simulated Annealing //Physical Letters A. 122. 1987. P. 157-162.

(3)Ingber L. Very fast simulated re-annealing // Mathematical and Computer Modelling. 12. 1989. P. 967-973.