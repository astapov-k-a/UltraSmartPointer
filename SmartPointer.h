/// @file SmartPointer.h
/// @brief Умный указатель, который определяет, включён ли класс в иерархию библиотеки со счётчиком ссылок,
///        или этот счётчик ссылок нужно отдельно создать
/// @disclaimer В продакшене я так не пишу, разве что если бы команда это поощряла.
///
/// @disclaimer №2 я отлаживал только шаблонную часть - в отлаженном виде будет позднее.
///                Прим. это уже вторая версия, рабочая, что называется Release Candidate 1.0 .
///                Для большего уровня корректности кода
///                нужно написать очень хороший тест, будет позднее.
/// @author Astapov K.A. +7(931)-29-17-0-16
/// @date 22/03/2016

#ifndef SMARTPOINTER_H
#define SMARTPOINTER_H

//#include <atomic> //мой gcc слишком стар :(
#include <qatomic.h>
#include <stdint.h>
#include <new>
#include <StaticLogic.h>


namespace example_lib {

template <typename Tn> struct DefaultTraits;
template <typename Tn, template <typename> class TraitsTn = DefaultTraits>
class SmartPointer;

typedef size_t Size;

/// @breif класс-счётчик ссылок
/// @details для наследования и агрегирования
class Counter {
 public:
  typedef void Type;
  //typedef std::atomic<Size> CounterInt;
  typedef QAtomicInt CounterInt; // не ставлю целью сделать потокобезопасный указатель, но пусть будет атомик на будущее
  typedef Type * Pointer;

    /**
     * @warning Этот класс - просто счётчик ссылок, который может являться частью данных.
     *          Это означает, что если при присвоении одному классу затрётся счётчик ссылок,
     *          то все указатели на него не валидны. Поэтому счётчики не копируются - 
     *          копирующие конструкторы и операторы присваивания у них пусты!!!
     **/
  Counter() : counter_(0) {}
  Counter(const Counter & to_copy) {Q_UNUSED(to_copy);} //нельзя автоматически копировать счётчик

    /**
     * @warning Этот класс - просто счётчик ссылок, который может являться частью данных.
     *          Это означает, что если при присвоении одному классу затрётся счётчик ссылок,
     *          то все указатели на него не валидны. Поэтому счётчики не копируются - 
     *          копирующие конструкторы и операторы присваивания у них пусты!!!
     **/
  Counter & operator=(const Counter & to_copy) {return * this;Q_UNUSED(to_copy);} //нельзя автоматически копировать счётчик

 protected:
  bool IncreaseReferenceCount() const {return counter_.ref();}
  bool DecreaseReferenceCount() const {return counter_.deref();}
  bool IsAnyReference() const {return counter_;}
  //Size GetReferenceCount() const {return counter_;}

 private:
  template <typename Tn, template <typename> class TraitsTn>
      friend class SmartPointer;

  mutable CounterInt counter_;
};

/// @brief корень иерархии - с функционалом или без, в зависимости от задач
class Base : public Counter {
  public:
    Base() : Counter() {}
    // @warning Копирующий конструктор пуст - нечего копировать в Base.  Главное, не мешать конструктору
    //          Counter ничего делать. Если Counter скопируестя, все умные указатели на Base станут невалидными!!!
    Base(const Base & to_copy) : Counter(to_copy) {Q_UNUSED(to_copy);} //нельзя автоматически копировать счётчик
    /** @remarks Деструктор не виртуальный. Ибо чтобы вставить в класс виртуальную таблицу,
                 утроить время доступа к некоторым функциям, нужна причина. Если цель - иерархия
                 высокого уровня, для принятия решений и обслуживания бизнес-логики, то это приемлемый расход
                 Если цель - служебная обвязка, сильно влияющая на скорость программы, и эта скорость ВАЖНА,
                 то виртуальность - крайняя мера.
                 Т.к. умный указатель - скорее ближе к движку программы, чем к бизнес-логике , по умолчанию полагаем второе
    **/    
    ~Base() {}    

    // @warning Копирующий оператор пуст - нечего копировать в Base.  Главное, не мешать оператору копирования
    //          Counter ничего делать. Если Counter скопируестя, все умные указатели на Base станут невалидными!!!
    Counter & operator=(const Counter & to_copy) {return * this;Q_UNUSED(to_copy);} //нельзя автоматически копировать счётчик
    Base       * Get()       {return this;} /// @для обеспечения совместимости с @ref SmartPointer
    Base const * Get() const {return this;} /// @для обеспечения совместимости с @ref SmartPointer
    template <class Tn>
    void Set(Tn & new_value) {
      volatile Tn * convert = dynamic_cast<Tn*>(this); //исключение, если в Set передан не наследник Base или данный класс не Tn
      *convert = new_value;
    }
    void DeleteData(DeleterFunction del_f) {Q_UNUSED(del_f);} /// @для обеспечения совместимости с @ref SmartPointer

   private:
    Base & GetData() {return *Get();}
};


namespace internal {

/**
  @brief контрольная часть для "чужих" или простых классов
  @remarks здесь везде используется наследование, а не using из c++11, потому что это "обрывает" цепочку простыней в названии классов при ошибках компиляции.
  @remarks контрольные части умного указателя:
  вариант "свой" (friend) - контрольная часть и есть объект, Control = Tn
  вариант "чужой" (foe) - указатель на некий класс, обычный вариант, Control = {Counter cnt; Tn * data;}
  вариант "простой" (simple) - Control = {Counter cnt; Tn data;}
  @todo я не стал инкапсулировать эти шаблоны в класс SmartPointer для простоты чтения, хотя там им самое место
  **/
template <typename Tn> class FoeOrSimpleControl : public Counter, public CompositeOrReference<Tn> {};

template <typename Tn, typename BaseTn>
class FriendOrFoeOrSimple
        : public StaticSwitch< Tn, FoeOrSimpleControl<Tn> , std::is_base_of<BaseTn, Tn>::value > {
};

// функторы создания контрольной части указателя - практически, куски мини-фабрики
/// @brief функтор создания контрольной части умного указателя для случая "простой"
template <typename Tn, typename ControlTn, template <typename> class Traits>
class MakeSimple{
 public:
  static ControlTn * Create() {
    ControlTn * re = Traits<Tn>::template Create<ControlTn>();
    ASSERTION(re, "Bad allocation", Exception)
    return re;
  }
  ControlTn * operator() () {return Create();}
};


/// @brief функтор создания контрольной части умного указателя для случая "чужой"
template <typename Tn, typename ControlTn, template <typename> class Traits>
class MakeFoe{
 public:
  static ControlTn * Create() {
    ControlTn * re = Traits<Tn>::template Create<ControlTn>();
    ASSERTION(re, "Bad allocation", Exception)
    if (re) {
      Tn * new_value =  static_cast<Tn*>( Traits<Tn>::template Create<Tn>() );
      ASSERTION(new_value, "Bad allocation", Exception)
      re->SetReference(new_value);
    }
    return re;
  }
  ControlTn * operator() () {return Create();}
};

/// #brief функтор создания контрольной части умного указателя для случая "свой"
template <typename Tn, typename ControlTn, template <typename> class Traits>
class MakeFriend{
 public:
  static ControlTn * Create() {
    ControlTn * re = static_cast<ControlTn*>( Traits<Tn>::template Create<Tn>() );
    ASSERTION(re, "Bad allocation", Exception)
    return re;
  }
  ControlTn * operator() () {return Create();}
};

template <typename Tn, typename ControlTn, template <typename> class Traits>
class MakeFoeOrFriend
    : public StaticSwitch<
                 MakeFoe<Tn, ControlTn, Traits>,
                 MakeFriend<Tn, ControlTn, Traits>,
                 !std::is_base_of<Base, Tn>::value > {
};

template <typename Tn, typename ControlTn, template <typename> class Traits>
class MakeFoeOrFriendOrSimple
    : public StaticSwitch<
                 MakeFoeOrFriend<Tn, ControlTn, Traits>,
                 MakeSimple<Tn, ControlTn, Traits>,
                 std::is_class<Tn>::value > {
};

} // namespace internal

template <typename Tn>
struct DefaultTraits {
  typedef const Tn *      Pointer;
  typedef       Tn * ConstPointer;
  typedef       Tn &      Reference;
  typedef const Tn & ConstReference;
  template <typename Type> static Type* Create() {return new (std::nothrow) Type;}
  template <typename Type> static void  Delete(Type * pointer) {
    delete pointer;
  }
  template <typename Type> static Type* CreateArray(Size size)      {return new (std::nothrow) Type[size];}
  template <typename Type> static void  DeleteArray(Type * pointer) {delete[] pointer;}
};

template <typename Tn, template <typename> class TraitsTn>
class SmartPointer
{
 public:
  typedef TraitsTn<Tn> Traits;
  typedef SmartPointer<Tn, TraitsTn> This;
  typedef Tn Type;
  typedef typename Traits::Pointer Pointer;
  typedef typename Traits::ConstPointer ConstPointer;
  typedef typename Traits::Reference Reference;
  typedef typename Traits::ConstReference ConstReference;
  class Maker;
  class Control;
  typedef Control * ControlPtr;

  SmartPointer() : ref_(nullptr) {}
  SmartPointer(const This & value) {value.ConstructReferenceAt(*this);}
  ~SmartPointer() {
    Dereference();
  }

  This & operator= (const This & value) {value.ConstructReferenceAt(*this);}
  Pointer      operator->()       {return static_cast<     Pointer>( get_control()->Get() );}
  ConstPointer operator->() const {return static_cast<ConstPointer>(     control()->Get() );}
  Reference      operator*()       {return static_cast<     Reference>( *get_control()->Get() );}
  ConstReference operator*() const {return static_cast<ConstReference>( *    control()->Get() );}
  operator bool() const {return control()->IsAnyReference();}

 protected:
  void Dereference();
  void IncreaseReference() const {control()->IncreaseReferenceCount();} // const потому, что логически указатель не меняется, только побитово
  void ConstructReferenceAt(This & to_construct) const;
  Control const *     control() const {return ref_;}
  Control       * get_control()       {return ref_;}
  void control (const Control * value) {ref_ = const_cast<Control *>(value);}
  Control const *     data() const {return     control().Get();}
  Control       * get_data()       {return get_control().Get();}

 private:
  template <typename Tn_, template <typename> class TraitsTn_>
    friend SmartPointer<Tn_, TraitsTn_> Make(); // привязываем мини-фабрику к указателю

  //friend This MakeCopy<Tn, TraitsTn>(const This &);

  Control * ref_;
};

/// @remarks - опять же, наследование вместо typedef сокращает простыни сообщений в случае ошибки компиляции
template <typename Tn, template <typename> class TraitsTn>
class SmartPointer<Tn, TraitsTn>::Control : public internal::FriendOrFoeOrSimple<Tn, Base> {
};

template <typename Tn, template <typename> class TraitsTn>
class SmartPointer<Tn, TraitsTn>::Maker
    : public internal::MakeFoeOrFriendOrSimple<
        Tn,
        Control,
        TraitsTn> {
};


template <typename Tn, template <typename> class TraitsTn>
void SmartPointer<Tn, TraitsTn>::Dereference() {
  if ( !control() ) return;
  if ( !get_control()->DecreaseReferenceCount() ) {
    typedef void (*LocalDelete) (Tn *);
    LocalDelete del_f = static_cast<LocalDelete>(&TraitsTn<Tn>::Delete); //берём указатель на шаблонную функцию-деаллокатор
    get_control()->DeleteData( (DeleterFunction)del_f );
    TraitsTn<Tn>::Delete(get_control());
    control(nullptr);
  }
};

template <typename Tn, template <typename> class TraitsTn>
void SmartPointer<Tn, TraitsTn>::ConstructReferenceAt(This & to_construct) const {
  IncreaseReference();
  to_construct.control( control() );
}


template <typename Tn, template <typename> class TraitsTn = DefaultTraits>
SmartPointer<Tn, TraitsTn> Make() {
   SmartPointer<Tn, TraitsTn> re;
   re.control( SmartPointer<Tn, TraitsTn>::Maker::Create() );
   ASSERTION( re.control() , "Bad alloc", Exception )
   re.IncreaseReference();
   return re;
}

} // namespace example_lib


#endif // SMARTPOINTER_H
