#include <iostream>
#include <string>

using namespace std;

template<class A, class B> struct Typelist {};

// берем код из презентации + http://loki-lib.sourceforge.net/html/a00653.html
template<class TList, template<class> class Unit>
class GenScatterHierarchy;

template<class T1, class T2, template<class> class Unit>
class GenScatterHierarchy<Typelist<T1, T2>, Unit>: 
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


template<class T>
struct Holder {
	T value;
};

using WidgetInfo = GenScatterHierarchy<
	Typelist<Typelist<int, double>, string>, 
	Holder>;


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
	WidgetInfo sample;

	Field<int>(sample).value = 666;
	cout << Field<int>(sample).value << endl;

	Field<double>(sample).value = 66.6;
	cout << Field<double>(sample).value << endl;

    Field<string>(sample).value = "six-six-six";
    cout << Field<string>(sample).value << endl;

	system("pause");
	return 0;
}
