///@author Astapov K.A. +7(931)-29-17-0-16
///@date 22/03/2016

#include <QtCore/QCoreApplication>
#include <SmartPointer.h>
#include <iostream>


class F{};
class D : public example_lib::Base {};

class E{
 public:
  ~E() {std::wcout << L"\nE destruct";}
};
class G : public example_lib::Base { public:  ~G() {std::wcout << L"\nG destruct";} };

int main(int argc, char *argv[])
{
  volatile int x = 0;
  typedef size_t Size;
  // можно в отладчике остановить и посмотреть, какие они изнутри
  CompositeOrReferenceIf<Size, true>  xxxx1;
  CompositeOrReferenceIf<Size, false> xxxx2;
  CompositeOrReferenceIf<F, true>  xxxx3;
  CompositeOrReferenceIf<F, false> xxxx4;
  CompositeOrReference<Size> xxx1;
  CompositeOrReference<F>    xxx2;
  example_lib::internal::FoeOrSimpleControl<Size> tt1;
  example_lib::internal::FoeOrSimpleControl<F   > tt2;
  example_lib::internal::FriendOrFoeOrSimple<Size, example_lib::Base> t1;
  example_lib::internal::FriendOrFoeOrSimple<F   , example_lib::Base> t2;
  example_lib::internal::FriendOrFoeOrSimple<D   , example_lib::Base> t3;
  example_lib::SmartPointer<int> ptr0;
  example_lib::SmartPointer<int> ptr1 = example_lib::Make<int>();
  example_lib::SmartPointer<E  > ptr2 = example_lib::Make<E  >();
  example_lib::SmartPointer<G  > ptr3 = example_lib::Make<G  >();
  std::wcout<<L"\n\nFun Test destruction done\n\n";
  //example_lib::SmartPointer<Size> xx;
  //example_lib::internal::FoeOrSimpleControl<Size> t;
  QCoreApplication a(argc, argv);
    
    return a.exec();
}
