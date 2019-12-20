#include <iostream>

template <class... Tail>
class ArgsPack;

template <>
// класс хранит аргументы и может применить к ним вызов функции, но не хранит саму функции (просто принимает на вход)
class ArgsPack<> {
public:
	//ArgsPack() {}
// принимает функцию и аргументы
	template<class Delegate, typename... Args>
		// у ArgsPack'а есть метод Call, который вызывает функцию от аргументов
	auto Call(const Delegate& delegate, Args&& ... args) -> decltype(delegate(args...))
	{
		return delegate(args...);
	}
};

// не можем конвертировать в вызове Call, поэтому делаем обертки

template<typename T>
T* ConvertToPtr(T& t) {
	return &t;
}
template<typename T>
T* ConvertToPtr(T* t) {
	return t;
}

template<typename T>
T& ConvertToRef(T& t) {
	return t;
}
template<typename T>
T& ConvertToRef(T* t) {
	return *t;
}

// реализация АргПака
template <class Head, class... Tail>
class ArgsPack<Head, Tail...> : public ArgsPack<Tail...> {
private:
	Head m_Param; // главный параметр Head

public:
	typedef ArgsPack<Tail...> __base;
	template<typename _Head, typename... _Tail>
	// "рекурсивный" конструктор аргпака от всех его аргументов
	ArgsPack(_Head&& head, _Tail&& ... tail)
		: __base(std::forward<_Tail>(tail)...),
		m_Param(std::forward<Head>(head)) {}

	template <class Delegate, class... Args>
	auto Call(const Delegate& delegate, Args/*&&*/ ... args) -> decltype(this->__base::Call(delegate, /*args... ,*/ std::forward<Args>(args)..., ConvertToRef(m_Param))) {
		return __base::Call(delegate, args..., ConvertToRef(m_Param));
	}

	template <class Delegate, class... Args>
	auto Call(const Delegate& delegate, Args/*&&*/ ... args) -> decltype(this->__base::Call(delegate, /*args... ,*/ std::forward<Args>(args)..., ConvertToPtr(m_Param))) {
		return __base::Call(delegate, args..., ConvertToPtr(m_Param));
	}
};

template<class TObject, typename TCE>
struct SimpleDelegate {
private:
	TObject& m_Object;
	TCE m_CallableEntity;

public:
	SimpleDelegate(TObject& object, const TCE& ptr)
		: m_Object(object), m_CallableEntity(ptr) {}

	template<typename... Params2>
	auto operator()(Params2... args) const
		-> decltype((m_Object.*m_CallableEntity) (args...)) {
		return (m_Object.*m_CallableEntity) (args...);
	}
};

template <class TCE>
struct CEWrapper {
private:
	TCE m_CallableEntity;

public:
	CEWrapper(const TCE& ptr) : m_CallableEntity(ptr) {}

	template<typename... Params2>
	auto operator()(Params2... args) const -> decltype(m_CallableEntity(args...)) {
		return m_CallableEntity(args...);
	}
};

// Функтор - класс, даем ему функцию и аргументы, потом вызываем функцию Callом из функтора
template<class TCallableEntity, class... Params>
class Functor {
private:
	ArgsPack<Params...> params_;
	TCallableEntity m_CallableEntity;
public:
	Functor(const TCallableEntity& ce, Params&& ... params) : m_CallableEntity(ce), params_(std::forward<Params>(params)...) {}

	template<class Object>
	auto Call(Object& object)-> decltype(params_.Call(SimpleDelegate<Object, TCallableEntity>(object, m_CallableEntity))) const {
		return params_.Call(SimpleDelegate<Object, TCallableEntity>(object, m_CallableEntity));
	}

	template<class TCE = TCallableEntity>
	auto Call() -> decltype(params_.Call(CEWrapper<TCE>(m_CallableEntity))) const {
		return params_.Call(m_CallableEntity);
	}

};

template<class TCallableEntity, typename...Params>
Functor<TCallableEntity, Params...> make_functor(const TCallableEntity& callable_entity, Params&& ... given_params) {
	return Functor<TCallableEntity, Params...>(callable_entity, std::forward<Params>(given_params)...);
}

int function(int x, float y, short z) {
	return (x + y) * (z - x) + y * z;
}
class Y {
public:
	int perem = 13;
};
class X {
public:
	float process(Y& y) {
		return y.perem / 3;
	}
};

int main() {
	
	// обычные функции
	auto functor1 = make_functor(&function, 1, 2, 3);
	int x = functor1.Call();
	// функции-члены
	auto functor2 = make_functor(&X::process, Y());
	X x2;
	float y = functor2.Call(x2);
	// лямбды
	Y y3;
	auto functor3 = make_functor([](Y*) {}, y3);
	functor3.Call();
	return 0;
}
