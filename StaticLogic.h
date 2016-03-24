/**
  @file StaticLogic.h
  @brief Файл, в котором в зависимости от условий статически выбирается тот или иной шаблон для генерации кода
  @details нужен для умного шаблона SmartPointer'а, который использует счётчик ссылок базового класса( если объект наследует базовый),
           и способен инкапсулировать короткие типы или POD'ы (в зависимости от настройки) в контрольную часть для экономии памяти
           и увеличения скорости доступа.
  @disclaimer В продакшене я так не пишу, разве что если бы команда это поощряла.
  @disclaimer №2 я отлаживал только шаблонную часть - в отлаженном виде будет позднее
///                Прим. это уже вторая версия, рабочая, что называется Release Candidate 1.0 .
///                Для большего уровня корректности кода
///                нужно написать очень хороший тест, будет позднее.
  @author Astapov K.A. +7(931)-29-17-0-16
  @date 22/03/2016
  **/
#ifndef STATICLOGIC_H
#define STATICLOGIC_H

#include <type_traits>

class Exception {};

/// @brief макрос обработки ошибки - в него можно подставить как исключение, так и int 3, так печать в лог - по ситуации. или ничего :)
#define ASSERTION(Condition, ErrorMessage, WhatObjectToThrow) {}

/// @brief выбирает класс из двух по условию
template <class A, class B, bool  Condition> class StaticSwitch;
template <class A, class B> class StaticSwitch< A, B, true>       : public A {};
template <class A, class B> class StaticSwitch< A, B, false>      : public B {};

typedef void (*DeleterFunction) (void *);

/// @brief если условие == 1, включает в себя (агрегирует композиционно) параметр; если условие == 0, создаёт на него ссылку. Предоставляет единый интерфейс доступа
/// @warning если Tn - класс, то у него подразумевается структура по умолчанию
template <typename Tn, bool Condition> class CompositeOrReferenceIf;


template <typename Tn> class CompositeOrReferenceIf<Tn,false> { //создаём ссылку на объект
 public:
  CompositeOrReferenceIf() : data_(nullptr) {}
  Tn       * Get()       {return data_;}
  Tn const * Get() const {return data_;}
  void Set(Tn & new_value) {GetData() = new_value;}
  void SetReference(Tn * value) {data_ = value;}
  void DeleteData(DeleterFunction del_f) {del_f( Get() );}///@remarks надо было сделать функтор удаления, так же, как для создания, по образцу MakeSimple и компания, но за неимением времени пусть будет так

 private:
  Tn & GetData() {return *Get();}
  Tn * data_;
};

#pragma pack(push, 1)
template <typename Tn> class CompositeOrReferenceIf<Tn,true> { // включаем объект - "делаем композицию"
public:
 Tn       * Get()       {return &data_;}
 Tn const * Get() const {return &data_;}
 void Set(Tn & new_value) {GetData() = new_value;}
 void DeleteData(DeleterFunction del_f) {Q_UNUSED(del_f);}

private:
 Tn & GetData() {return *Get();}
 Tn data_;
};
#pragma pack(pop)


/// @brief наследует параметр, если он - класс. в ином случае, агрегирует его. Предоставляет единый интерфейс доступа
/// @details служит для экономии. Допустим, вам нужен умный указатель на int. Вы создаёте контрольную структуру с указателем на
/// int и счётчиком ссылок. Плюс указатель на контрольную структуру в копируемой части SmartPointer'а. Получаем 4 инта вместо одного -
/// сам int данных одна штука, два указателя и один счётчик. Если много указателей на int, будет мощный овердрафт
/// В данном случае этот шаблон разберётся, что перед ним указатель на простой тип

/** @remarks условие "не-класс" можно уточнять в зависимости от условий задачи - "не-класс или POD, но не union",
             "не-класс или POD с sizeof < const_lim_size", например.
             Это всего лишь простой образец того, как это можно сделать
**/
template <typename Tn> class CompositeOrReference : public CompositeOrReferenceIf<Tn, ! std::is_class<Tn>::value > {};


#endif // STATICLOGIC_H
