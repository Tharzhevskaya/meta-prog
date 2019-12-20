#include <iostream>
#include <string>

using namespace std;

template<class A, class B> struct Typelist {};

// берем код из презентации + http://loki-lib.sourceforge.net/html/a00653.html
template<class TList, template<class> class Unit>
class GenScatterHierarchy;

// создаем иерархию GenScatterHierarcy
template<class T1, class T2, template<class> class Unit>
class GenScatterHierarchy<Typelist<T1, T2>, Unit>: 
// наследуемся от левого и правого поддерева
	public GenScatterHierarchy<T1, Unit>, 
	public GenScatterHierarchy<T2, Unit> {
public:
	using TList = Typelist<T1, T2>;
	
	using LeftBase = GenScatterHierarchy<T1, Unit>;
	using RightBase = GenScatterHierarchy<T2, Unit>;

	template <class T>
	struct Rebind
	{
		typedef Unit<T> Result;
	};
};

// лист дерева
template<class AtomicType, template<class> class Unit>
class GenScatterHierarchy : public Unit<AtomicType> {
public:
	using LeftBase = Unit<AtomicType>;

	template <class T>
	struct Rebind
	{
		typedef Unit<T> Result;
	};
};

struct NullType {};
template<template<class> class Unit>
class GenScatterHierarchy<NullType, Unit > {
	template <class T> 
	struct Rebind
	{
		typedef Unit<T> Result;
	};

};

// Шаблонный класс Holder, которому мы хотим по очереди передать int, strint
// Хотим иметь доступ одним и тем же методом к объектам разных типов
template<class T>
struct Holder {
	T value;
};

using WidgetInfo = GenScatterHierarchy<
	Typelist<Typelist<int, double>, string>, 
	Holder>;

// поле шаблона функции
// Доступ к полю в объекте типа, созданного с помощью GenScatterHierarchy
// obj - это объект типа H, созданный с помощью GenScatterHierarchy,
// T - это тип в списке типов, используемый для генерации H
// Поле <T> (объект)
// возвращает ссылку на Unit <T>, где Unit - шаблон, используемый для генерации H
template <class T, class H>
typename H::template Rebind<T>::Result& Field(H& sample)
{
	return sample;
}

template <class T, class H>
const typename H::template Rebind<T>::Result& Field(const H& sample)
{
	return sample;
}

int main() {
	WidgetInfo sample; // создали объект класса, который унаследовался от Holder<string>, Holder<double>, и ...<string>

	// метод Field (функция доступа к члену класса WigetInfo по типу) нам вернет указатель на 
	Field<int>(sample).value = 666;
	cout << Field<int>(sample).value << endl;

	Field<double>(sample).value = 66.6;
	cout << Field<double>(sample).value << endl;

    Field<string>(sample).value = "six-six-six";
    cout << Field<string>(sample).value << endl;

	system("pause");
	return 0;
}
