///@file test file
///@brief only for test
///@author Astapov K.A. +7(931)-29-17-0-16
///@date 22/03/2016

#include <QtCore/QCoreApplication>
#include <SmartPointer.h>
#include <iostream>


class F{};
class D : public example_lib::Base {
 public:
  D() : x(56635) {}
  int x;
};

class E{
 public:
  E() : x(255) {}
  ~E() {std::wcout << L"\nE destruct";}
  int x;
};
class G : public example_lib::Base {
 public:
 G() : x(65535) {}
 int x;
 ~G() {std::wcout << L"\nG destruct";}
};

template <typename Tn> void Test(example_lib::SmartPointer<Tn> ptr) {
  Tn a = *ptr;
  Tn c;
  //c. x=(17);
  example_lib::SmartPointer<Tn> ptr2 = ptr;
  const Tn * b =ptr.operator ->();
  a = *b;
  *ptr2 = c;
  Q_UNUSED(a);
  Q_UNUSED(b);
  Q_UNUSED(c);
}
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
  Test(ptr1);
  Test(ptr2);
  Test(ptr3);
  E to_copy_e;
  G to_copy_g;
  to_copy_e.x = 17;
  to_copy_g.x = 18;
  *ptr2 = to_copy_e;
  *ptr3 = to_copy_g;
  std::wcout<<L"\n\nFun Test destruction done\n\n";
  //example_lib::SmartPointer<Size> xx;
  //example_lib::internal::FoeOrSimpleControl<Size> t;
  //QCoreApplication a(argc, argv);
    
  //return a.exec();
}
