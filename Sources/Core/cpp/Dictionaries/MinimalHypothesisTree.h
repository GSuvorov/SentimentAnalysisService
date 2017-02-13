#pragma once

namespace SS{ 
namespace Dictionary{
namespace MinimalHypothesisTree{

using namespace SS::Dictionary;


/**
   Модуль реализует построение "Минимального дерева вывода гипотез". Это дерево по которому можно определить 
   с большой долей вероятности морфологическую информацию по любому слову (гипотезу). При этот для всех слов по 
   которым стироется дерево можно всегда определить точную морфологическую информациию. Т.е. есть
   исходный набор слов и морфологическая информация по ним, которые отражают  
   подавляющее большинство возможных способов морфообразования и мы строим минимальное дерево вывода для этих 
   слов. По этому дереву, очевидно, можно определять морфологическую информацию о новом слове.

   Дерево строится следующим образом, каждый узел дерева это отдельная буква в слове, у дерева много корней (и строго говоря этот граф не дерево).
   Каждый корень хранит букву на которую может оканчиваться слово в словаре, а каждый потомок это буква в слове следующая 
   перед текущей. Т.е. мы храним слова в обратном порядке в дереве в котором все потомки одного узла являются всеми возможными
   буквами которые могут следовать после части слова составленного из текущего узла и всех его радителей:
      Если в словаре 3 слова "дом", "сом", "кот" то дерево будет следующим:
                                 м     т
                                 |     |
                                 о     о
                                / \    |
                               д   с   к
   
   В каждом листе дерева хранится и иногда в отедельном узле хранится морфологическая информация о слове.
   Если в узле есть морфоинформация то слово от корня до этого узла можно считать существующим в словаре.
   Т.О. При определении гипотезы необходимо найти самое длинное совпадающее с конца слово в дереве, его морфологическая информация 
   и будет гипотезой.

   После того как дерево было построено, производится оптимизация по след-му алгоритму:
      Для каждого узла начиная с самых удалённых от корня и поднимаясь постепенно к корню:
         Если узел является листом то переходим к следующему узлу

         Если у узла нет морфологической инф-ии (МИ), то выбираем из МИ потомков ту которая встречается наиболее часто
         и приписываем её текущему узлу

         Над всеми потомками текущего узла у которых МИ совпадает с текущем узлом проводим след-ие операции:
         Если это лист то удаляем его, если не лист обнуляем МИ.
*/

///////////////////////////////////////////////////////////////////////////////
// Реализация контейнера который хранит дерево со множеством корней и позволяет 
// к каждому узлу добавить некоторую инфрмацию
// K - Ключ дерева, гарантированно уникальный у всех потомков узла
// V - Дополнительная информация об узле
///////////////////////////////////////////////////////////////////////////////
template<class K, class V>   
class CTree
{   
public:
   typedef K TKey;     //Ключ по которому строится дерево
   typedef V TValue;   //Значение которое приписывается каждому листу
   class CNode;

private:
   typedef std::map<TKey, CNode> TChildrenStorage;

public:
   typedef  typename TChildrenStorage::iterator TNodeIterator;               //Итератор листа, объект по итератору является std::pair<TKey, CNode>
   typedef  typename TChildrenStorage::const_iterator TConstNodeIterator;    //Константный итератор листа

public:
   //Узел дерева
   class CNode
   { 
      TChildrenStorage m_ChildrenStorage;
      TValue m_Value;

   public:
      explicit CNode( TValue Value = TValue() ): m_Value(Value) {};
       
      //Значение узла
      const TValue &GetValue() const{ return m_Value; }
      void SetValue( const TValue &Value ){ m_Value = Value; }

      //Является ли листом (нет потомков)
      bool IsLeaf() const { return m_ChildrenStorage.empty(); }

      //Начало и конец интервала дочерних узлов
      TNodeIterator Begin(){ return m_ChildrenStorage.begin(); }
      TNodeIterator End()  { return m_ChildrenStorage.end(); }
      TConstNodeIterator Begin() const { return m_ChildrenStorage.begin(); }
      TConstNodeIterator End() const   { return m_ChildrenStorage.end(); }

      //Добавить дочерний узел только в том случае если нет ещё дочерних узлов с ключём Key.
      //Возвращает в первой компоненте пары итератор дочернего узла с ключём Key (либо только что
      //вставленного, либо существовавшего до вставки). Вторая компонента показывает была ли вставка.
      std::pair<TNodeIterator, bool> Insert( const TKey &Key, const CNode &Node = CNode() )
      { return m_ChildrenStorage.insert( TChildrenStorage::value_type(Key, Node) ); }

      //Удалить дочерний узел и всех его потомков
      void Erase( TNodeIterator Pos ){ m_ChildrenStorage.erase(Pos); }
      void Erase( TNodeIterator Begin ,TNodeIterator End ){ m_ChildrenStorage.erase(Begin, End); }

      //Поиск потомка с ключём Key. Возвр End в случаи неудачного поиска
      TConstNodeIterator Find( const TKey &Key ) const { return m_ChildrenStorage.find(Key); }
      TNodeIterator Find( const TKey &Key ) { return m_ChildrenStorage.find(Key); }

      //Удалить все потомки
      void Clear(){ m_ChildrenStorage.clear(); }
   };

private:
   CNode m_RootNode;

public:
   //Оператор копирования и присваивания сгенерированные компилятором вполне подходят
   
   //Виртуальный корень дерева (существует даже в пустом дереве)
   const CNode &Root() const { return m_RootNode; }
   CNode &Root() { return m_RootNode; }
};
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// Класс который выполняет построение Минимального дерева вывода
///////////////////////////////////////////////////////////////////////////////
class CMinimalHypothesisTree
{
public:   
   
   //Структура позволяет хранить всю морфоинформацию о слове
   struct SMorhpo//: public CObjectCount<SMorhpo>
   {
      typedef std::set<TIDType> TIDTypeStotage;

      TIDTypeStotage m_IDTypes;

      friend bool operator ==( const SMorhpo& M1, const SMorhpo& M2 )
      { return M1.m_IDTypes == M2.m_IDTypes; } 
      
      friend bool operator !=( const SMorhpo& M1, const SMorhpo& M2 )
      { return M1.m_IDTypes != M2.m_IDTypes; } 
      
      
      friend bool operator<( const SMorhpo& M1, const SMorhpo& M2 )
      { return M1.m_IDTypes < M2.m_IDTypes; } 
   };

   typedef CAutoRefCount<SMorhpo> TMorphoCreator;
   typedef wchar_t TChar;

private:
   struct SMorphoLess: public std::binary_function<TMorphoCreator::SmartPtr, TMorphoCreator::SmartPtr, bool>
   {
      bool operator()( TMorphoCreator::SmartPtr P1, TMorphoCreator::SmartPtr P2 ) const
      {
         APL_ASSERT( P1 != 0 && P2 != 0 );           
         return *P1 < *P2;
      }
   };
 
   
   //Класс позволяет вести статисктику, по морфоинформации. 
   //Позволяет сопоставить Морфоинформации некоторый коэффициент, клоторый можно увеличивать или 
   //уменьшать и находить морфоинформацию с максимальным коэффициентом.
   class CMorphoStatistic                                 
   {
      typedef std::multimap<size_t, TMorphoCreator::SmartPtr> TPriorityStorage; //Коэффициент каждой морфоинформации      
      typedef std::map<TMorphoCreator::SmartPtr, TPriorityStorage::iterator, SMorphoLess> TMorphoStorage; //Хранимая морфоинформация
   
   private:
      TPriorityStorage m_PriorityStorage;
      TMorphoStorage m_MorphoStorage;

   public:
      bool Empty() const { return m_PriorityStorage.empty(); }
      void Clear() { m_PriorityStorage.clear(); m_MorphoStorage.clear(); }

      //Изменить коэффициент морфоинформации на D. (Если такой нет то добавляется новая и количество становится D)
      void Change( TMorphoCreator::SmartPtr Ptr, size_t D )
      {
         APL_ASSERT( Ptr != 0 );

         TMorphoStorage::iterator I = m_MorphoStorage.find(Ptr);

         if( I == m_MorphoStorage.end() )
         {
            APL_ASSERT( std::find_if(m_PriorityStorage.begin(), m_PriorityStorage.end(),
               std::bind2nd(
                  binary_compose_1arg( 
                     std::equal_to<SMorhpo>(), 
                     unary_compose_1arg( pointer_deref_t<TMorphoCreator::SmartPtr, SMorhpo>(), select_second_t<TPriorityStorage::value_type>() ),
                     pointer_deref_t<TMorphoCreator::SmartPtr, SMorhpo>() 
                  ),
                  Ptr
               ) ) == m_PriorityStorage.end()
            );

            TPriorityStorage::iterator J = m_PriorityStorage.insert( TPriorityStorage::value_type(D, Ptr) );
            APL_CHECK( m_MorphoStorage.insert(TMorphoStorage::value_type(Ptr, J)).second );
         }
         else
         {            
            std::pair<size_t, TMorphoCreator::SmartPtr> VT(*I->second);
            VT.first += D;
            m_PriorityStorage.erase(I->second);
            
            //std::pair<TPriorityStorage::iterator, bool> Rez( m_PriorityStorage.insert(VT) );  
            //APL_ASSERT( Rez.second );
            I->second = m_PriorityStorage.insert(VT);
         }
      }

      //Морфоинформация с максимальным коэфф-ом.
      TMorphoCreator::SmartPtr GetMax() const { APL_ASSERT(!Empty()); return m_PriorityStorage.rbegin()->second; }

      //Добавить информацию из другого контейнера статистики
      void AddStatistic( const CMorphoStatistic &MS )
      {
         for( TPriorityStorage::const_iterator I = MS.m_PriorityStorage.begin(); I != MS.m_PriorityStorage.end(); ++I )
            Change( I->second, I->first );
      }
   };

   //Тоже что и CMorphoStatistic, но позволяет только инкрементировать значения на 1
   class CMorphoStatisticLite                                 
   {
      typedef std::map<TMorphoCreator::SmartPtr, size_t, SMorphoLess> TMorphoStorage; //Хранимая морфоинформация

   private:
      TMorphoStorage m_MorphoStorage;
      TMorphoStorage::iterator m_CurMaxIter;

   public:
      CMorphoStatisticLite(): m_CurMaxIter(m_MorphoStorage.end()) {}
      bool Empty() const { return m_MorphoStorage.empty(); }
      void Clear() {  m_MorphoStorage.clear(); m_CurMaxIter = m_MorphoStorage.end(); }

      //Изменить коэффициент морфоинформации на 1. (Если такой нет то добавляется новая и количество становится 1)
      void Increment( TMorphoCreator::SmartPtr Ptr )
      {
         APL_ASSERT( Ptr != 0 );

         TMorphoStorage::iterator I = m_MorphoStorage.find(Ptr);

         if( I == m_MorphoStorage.end() )
         {          
            std::pair<TMorphoStorage::iterator, bool> Rez( m_MorphoStorage.insert(TMorphoStorage::value_type(Ptr, 1)) );
            
            APL_ASSERT( Rez.second );
            I = Rez.first;
         }
         else
         {    
            ++I->second;
         }

         if( m_CurMaxIter == m_MorphoStorage.end() || I->second > m_CurMaxIter->second )
            m_CurMaxIter = I;   
      }

      //Морфоинформация с максимальным коэфф-ом.
      TMorphoCreator::SmartPtr GetMax() const { APL_ASSERT(!Empty()); return m_CurMaxIter->first; }
   };
   
   typedef CTree<TChar, TMorphoCreator::SmartPtr>  TTree;

public:

   //Курсор позволяющий пройти по всем словам из дерева
   class SCursor
   {
   public:    
      typedef std::basic_string<TChar> TWord;

   private:
      friend CMinimalHypothesisTree;
      typedef std::deque<std::pair<TTree::TConstNodeIterator, TTree::TConstNodeIterator> > TIterStack; 
   
   private:
      TIterStack IterStack;                        //Стек с итераторами текущего положения в дереве
      TWord Word;                                  //Само слово
      TMorphoCreator::ConstSmartPtr Ptr;           //Указатель на морфоинформацию

   public:
      //Текущее слово
      const TWord &GetWord() const { return Word; }
      const TMorphoCreator::ConstSmartPtr &GetPtr() const { return Ptr; }

      //Закончился ли поиск
      bool End() const { return IterStack.empty(); }
   };

   //Статистические данные по дереву
   struct SStatistic
   {
      typedef size_t UnsignedT;
      UnsignedT WordsCount;        //Количество слов в дереве
      UnsignedT CharCount;         //Количество букв во всех словах в дереве
      UnsignedT NodeCount;         //Количество узлов в дереве
      UnsignedT TypesCount;        //Суммарное количество типов которое можно вывести из узла (сумма SMorhpo.m_IDTypes)

      void Clear()
      {
         WordsCount = 0;
         CharCount = 0; 
         NodeCount = 0; 
         TypesCount = 0;
      }
   };


private:
   TTree m_Tree;

private:
   //Проверить что между итераторами расстояние в точности 1
   //работает в случае итераторов не произвольного доступа быстрее чем 
   //std::distance(I1, I2) == 1
   template< class InputIterator >
   static bool DistanceEqual1( InputIterator I1, InputIterator I2 )
   {
      return I1 != I2 && ++InputIterator(I2) == I2;
   }
   
   //Реализация оптимизации дерева. Вызывается рекурсивно.
   //Node - Текущий узел
   //MS - Записывается статистика по морфоинформации о всех путях идущих от Node  
   void OptimizeTreeImpl( TTree::CNode &Node );
   //TMorphoCreator::SmartPtr OptimizeTreeImpl( TTree::CNode &Node );

   //Реализация: Собрать статистические данные по дереву, вызывается рекурсивно
   //Depth - глубина  текущего узла дерева (начинается с 1)
   void CalcStatisticDataImpl( const TTree::CNode &Node, SStatistic &Statistic, size_t Depth ) const;

public:  
   
   //Добавить строку [Begin, End) морфоинформацию по ней в дерево.
   //[Begin, End) - сма строка
   //Ptr - Морфоинформация по ней.
   //Возвр: Удалось ли вставить слово (если такое слово уже есть вставка не происходит)
   template<class InputIterator>
   bool AddString( InputIterator Begin, InputIterator End, TMorphoCreator::SmartPtr Ptr )
   {
      TTree::CNode *pNode = &m_Tree.Root();
      
      std::pair<TTree::TNodeIterator, bool> ResultPair;

      for( ; Begin != End; ++Begin )
      {
         ResultPair = pNode->Insert( *Begin  );

         pNode = &ResultPair.first->second;
      }

      if( pNode->GetValue() != 0 )
         return false;

      pNode->SetValue(Ptr);

      return true;
   }
   
   //Опредилить морфоинформацию по слову [Begin, End)или если таковой нет возвра-ся 0
   template<class InputIterator>
   TMorphoCreator::ConstSmartPtr GetMorpho( InputIterator Begin, InputIterator End ) const
   {
      TMorphoCreator::ConstSmartPtr RetValue;
      const TTree::CNode *pNode = &m_Tree.Root();
      TTree::TConstNodeIterator I;

      for( ; Begin != End; ++Begin )
      {
         I = pNode->Find( *Begin  );

         if( I == pNode->End() )
            break;

         pNode = &I->second;

         if( pNode->GetValue() != 0 )
            RetValue = pNode->GetValue();
      }

      return RetValue;
   }

   //Инициализация сканирования строк
   //И поиск первого слова если есть. Если в дереве нет ни одного слова то Cursor.End() == true
   void ScanInit( SCursor &Cursor ) const;

   //Найти следующее слово, чтобы найтивсеп слова необхордимо один раз вызвать ScanInit, а затем пока
   //не Cursor.End() == false вызывать ScanNext
   void ScanNext( SCursor &Cursor ) const;

   //Оптимизировать дерево, т.е. создать минимальное дерево поиска, по правилам указанным выше.
   void OptimizeTree()
   { 
      for( TTree::TNodeIterator I = m_Tree.Root().Begin(); I != m_Tree.Root().End(); ++I )
         OptimizeTreeImpl(I->second); 
   }

   //Собрать статистические данные по дереву
   void CalcStatisticData( SStatistic &Statistic ) const
   { 
      Statistic.Clear();

      for( TTree::TConstNodeIterator I = m_Tree.Root().Begin(); I != m_Tree.Root().End(); ++I )
         CalcStatisticDataImpl(I->second, Statistic, 1); 
   }

   //Полная очистка
   void Clear(){ m_Tree.Root().Clear(); }
};


///////////////////////////////////////////////////////////////////////////////
// Паттерн Фасад.
// Высокоуровневая оболачка для построения Минимального дерева вывода
///////////////////////////////////////////////////////////////////////////////
class CFacade
{
   typedef CMorphoTypesSearchCreateManager::TPointer TMorphoTypesSearchPtr;

private:
   CMinimalHypothesisTree m_MinimalHypothesisTree;

   IAMConverterMorpho  *m_pAMConverter;
   TMorphoTypesSearchPtr m_pMorphoTypesSearch;
   TDictNumber m_DictNumber;

private:
   //Можно ли добалять такой IDType
   bool CanAddIDType( TIDType IDType );

public:
   CFacade(): m_pAMConverter(0), m_DictNumber(DATA_TYPE::NAMES::ednUnknown) { }
   
   //Инициализировать данные
   void Init( IAMConverterMorpho *pAMConverter, DBMS::IDataBase *pDataBase, TDictNumber DictNumber );

   //Загрузить дерево из файла представляющего из себя последовательность ОТСОРТИРОВАННЫХ строк след. формата: 
   //    <слово в обраном порядке букв><пробельный символ(ы)><IDType>
   void LoadFromFile( const char *pFileName );

   //Добавить в дерево слово с набором морфоинформации (список IDType).
   //Необходимо вызвать для каждого уникального слова словаря и морфоинформации по нему.
   //    Str - Добавляемое слово. Представлено в инвертированном виде (вместо "слон" необходимо передать "нолс")
   //    TypesList - Набор IDType которые могут быть выведены по слову Str
   //    Вовращает: Удалось ли добавить слово (если такое слово уже есть в дереве или если все IDType 
   //               в TypesList не являются продуктивными, добавление не происходит)
   bool AddStringReverse( const std::wstring &Str, const TTypesList &TypesList );

   //Протестировать что из текущего дерева выводится такие же типы как и из файла
   bool TestTree( const char *pFileName ) const;

   //Сохранить дерево в файл сходного формата с LoadFromFile
   void SaveToFileOriginal( const char *pFileName ) const;

   //Сохранить дерево в файл в котором все IDType идут в одну строку
   //TotalWordCount - Количество слов
   //TotalCharCount - Количество символов во всех словах
   void SaveToFileStrict( const char *pFileName, size_t &TotalWordCount, size_t &TotalCharCount ) const;

   //Сохранить дерево в таблицу со структурой:
   //       #DATA_FIELD_INFO = "ID"        : INT32              // IDType
   //       #DATA_FIELD_INFO = "Word"      : WSTRING - NO_FIND  // Литеральное представление окончания
   //При этом слова записываются в обратном порядке следования букв (т.е. в таблицу попадают оргинальные слова)
   void SaveToTable( DBMS::IDataTable *pTable ) const ;

   //Оптимизировать дерево
   void Optimize() { m_MinimalHypothesisTree.OptimizeTree(); }

   typedef CMinimalHypothesisTree::SStatistic TStatistic;
   
   void CalcStatisticData( TStatistic &Statistic ) const { m_MinimalHypothesisTree.CalcStatisticData(Statistic); }

   void Clear() { m_MinimalHypothesisTree.Clear(); }
};


class CMinimalHypothesisTreeBuilder: public IMinimalHypothesisTreeBuilder
{
   CDBMSPtrsManager m_DBMSPtrsManager;
   DBMS::IDataBase *m_pMorphoDB;
   CFacade m_Facade;

public:
   CMinimalHypothesisTreeBuilder(): m_pMorphoDB(0) {}

   bool Init( TDictNumber DictNumber );
   bool AddReverseWord( const std::wstring* pwReverseWord, const TTypesList* pTypesList );
   void Build();

public:
   RELEASE_IMPL()

   BEGIN_QI_MAP()
      QI_MAP_INHERIT_ENTRY( IID_Base, IBase )
      QI_MAP_INHERIT_ENTRY( IID_IMinimalHypothesisTreeBuilder, IMinimalHypothesisTreeBuilder )
   END_QI_MAP()
};

} //namespace MinimalHypothesisTree
} //namespace Dictionary
} //namespace SS
