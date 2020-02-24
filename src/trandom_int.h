#ifndef TRANDOM_INT_H
#define TRANDOM_INT_H

#include <random>
#include <limits>

///Синглтон - генератор случайных беззнаковых целых с равномепным распределением по методу Mersenne Twister
class TRandomInt {
private:
  std::tr1::mt19937 RE;
  std::tr1::uniform_int<unsigned long> * URUL;

  ///Private constructor
	TRandomInt() {URUL = 0;}                       
  ~TRandomInt() {}  
	///Prevent copy-construction
	TRandomInt(const TRandomInt&);                 
  ///Prevent assignment
	TRandomInt& operator=(const TRandomInt&);      
public:
	///Получить экземпляр
  static TRandomInt& Instance()
  {
    static TRandomInt random_int;
    return random_int;
  }
	///Инициализация равномерного распределения
  void Initialize(unsigned long int s = 0)
  {
		if (s != 0) RE.seed(s); else RE.seed((unsigned int)time(0));
    URUL = new std::tr1::uniform_int<unsigned long>(0, std::numeric_limits<unsigned long>::max());
  };
	///Получить случайный integer
  unsigned long operator () () {
    if (URUL == 0) Initialize();
    return (*URUL)(RE);
  }
};

///Глобальная функция для упрощения вызова
inline unsigned long RandomInt()
{
  TRandomInt * ri = &(TRandomInt::Instance());
  return (*ri)();
};

#endif