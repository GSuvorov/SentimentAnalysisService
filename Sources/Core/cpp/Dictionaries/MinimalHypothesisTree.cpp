#include "stdafx.h"
#include "MinimalHypothesisTree.h"

namespace SS{ 
namespace Dictionary{
namespace MinimalHypothesisTree{


///////////////////////////////////////////////////////////////////////////////
// class CMinimalHypothesisTree
///////////////////////////////////////////////////////////////////////////////
void CMinimalHypothesisTree::ScanInit( SCursor &Cursor ) const
{
   Cursor.IterStack.clear();   
   Cursor.Word.clear();
   Cursor.Ptr.Release();

   Cursor.IterStack.push_back( SCursor::TIterStack::value_type(m_Tree.Root().Begin(), m_Tree.Root().End()) );
   ScanNext(Cursor);
}
///////////////////////////////////////////////////////////////////////////////

void CMinimalHypothesisTree::ScanNext( SCursor &Cursor ) const
{
   if( Cursor.IterStack.empty() )
      return;

   TTree::TConstNodeIterator CurI;
   for(;;)
   {
      //Сначала доходим до листа, т.е. залезаем так глубоко как возможно в самую ... ;)
      //Разворачиваем стек и попутно пытаемся найти подходящие слова,т.е. те узлы у 
      //которых есть морфоинформация
      while( Cursor.IterStack.back().first != Cursor.IterStack.back().second )
      {
         CurI = Cursor.IterStack.back().first;  
         
         Cursor.Word.push_back(Cursor.IterStack.back().first->first);

         Cursor.IterStack.push_back( 
            SCursor::TIterStack::value_type(
               Cursor.IterStack.back().first->second.Begin(), Cursor.IterStack.back().first->second.End()
            ) 
         );

         //Если у этого узла есть морфоинформация то мы нашли слово
         if( CurI->second.GetValue() != 0 )
         {
            Cursor.Ptr = CurI->second.GetValue();
            return;
         }
      }

      //Мы дошли до листа, теперь возвращаемся наверх к корню дерева, сворачивая стек
      //Т.е. находим перый узел который мы ещё не просмотрели
      do
      {
         Cursor.IterStack.pop_back();
         
         if( Cursor.IterStack.empty() )
            return;

         Cursor.Word.resize(Cursor.Word.size() - 1);
      }
      while( Cursor.IterStack.back().first == Cursor.IterStack.back().second );

      //Если мы сюда дошли то мы нашли следующий подходящий узел 
      ++Cursor.IterStack.back().first;
   }
}

///////////////////////////////////////////////////////////////////////////////
void CMinimalHypothesisTree::OptimizeTreeImpl( TTree::CNode &Node )
{
   if( Node.IsLeaf() )
   {
      //Если это лист то ничего не делаем
      APL_ASSERT( Node.GetValue() != 0 );

      return;
   }
    
   TTree::TNodeIterator I, J;

   if( Node.GetValue() == 0 )
   {
      CMorphoStatisticLite MS;
      
      //Мы можем поднять какую либо морфоинформацию наверх, 
      //рекурсивно вызываем функцию оптимизации и собираем статистику
      for( I = Node.Begin(); I != Node.End(); ++I )
      {
         OptimizeTreeImpl( I->second );
         APL_ASSERT( I->second.GetValue() != 0 ); //После оптимизации узла он не может иметь пустую морфоинф-ю
         MS.Increment(I->second.GetValue());
      }

      APL_ASSERT( !MS.Empty() );

      //Поднимаем самую попуярную морфоинф-ю наверх, т.е. присваеваем её текущему узлу
      APL_ASSERT(MS.GetMax() != 0);
      Node.SetValue(MS.GetMax());
   }
   else
   {
      //Т.к. текущий узел уже содержит м-ю нельзя ничего поднять наверх. Посто вызываем рекурсивно оптимизацию.
      for( I = Node.Begin(); I != Node.End(); ++I )
      {
         OptimizeTreeImpl( I->second );
         APL_ASSERT( I->second.GetValue() != 0 ); //После оптимизации узла он не может иметь пустую морфоинф-ю
      }
   }

   //Над всеми потомками текущего узла у которых м-я совпадает с текущем узлом проводим след-ие операции.
   //Если это лист то удаляем его, иначе обнуляем м-ю.
   for( I = Node.Begin(); I != Node.End(); )
   {
      J = I++;
      
      if( *J->second.GetValue() == *Node.GetValue() )
      {
         if( J->second.IsLeaf() )
            Node.Erase( J );
         else
            J->second.SetValue(TMorphoCreator::SmartPtr(0));
      }
   }
}
///////////////////////////////////////////////////////////////////////////////

void CMinimalHypothesisTree::CalcStatisticDataImpl( const TTree::CNode &Node, SStatistic &Statistic, size_t Depth ) const
{
   //Если мы сюда пришли то это во первых узел :)
   ++Statistic.NodeCount;

   if( Node.GetValue() != 0 )
   {
      ++Statistic.WordsCount;
      Statistic.TypesCount += Node.GetValue()->m_IDTypes.size();
      Statistic.CharCount += Depth;
   }

   for( TTree::TConstNodeIterator I = Node.Begin(); I != Node.End(); ++I )
      CalcStatisticDataImpl( I->second, Statistic, Depth + 1 );
}

///////////////////////////////////////////////////////////////////////////////
// class CFacade
///////////////////////////////////////////////////////////////////////////////
void CFacade::Init( IAMConverterMorpho *pAMConverter, DBMS::IDataBase *pDataBase, TDictNumber DictNumber )
{
   APL_ASSERT_PTR( pAMConverter );

   Clear();

   m_pAMConverter = pAMConverter;
   m_pMorphoTypesSearch = CMorphoTypesSearchCreateManager::Create()->Construct(pDataBase, NULL);
   m_DictNumber = DictNumber;
}
///////////////////////////////////////////////////////////////////////////////

bool CFacade::CanAddIDType( TIDType IDType )
{
   APL_ASSERT_PTR(m_pAMConverter);

   CMorphoFeature MorphoFeature;
   std::list<SMorphoType> MorphoTypes;

   //Необходимо исключить такие IDType которые не являются ни существительным, ни прилагательным, ни глаголом
   m_pMorphoTypesSearch->GetMorphoTypesListByIDType( IDType, &MorphoTypes );

   for( std::list<SMorphoType>::iterator I = MorphoTypes.begin(); I != MorphoTypes.end(); ++I )
   {
      m_pAMConverter->EnCode( m_DictNumber, I->m_MorphoTypeInfo.m_MorphoInfo, I->m_SpecialMorpho, &MorphoFeature );

      if( 
         MorphoFeature.m_PartOfSpeechTypes.IsContain(SS::Core::Features::Values::PartOfSpeechTypes.postNoun) ||
         MorphoFeature.m_PartOfSpeechTypes.IsContain(SS::Core::Features::Values::PartOfSpeechTypes.postAdjective) ||
         MorphoFeature.m_PartOfSpeechTypes.IsContain(SS::Core::Features::Values::PartOfSpeechTypes.postVerb)
      )
         return true;
   }

   return false;
}
///////////////////////////////////////////////////////////////////////////////

void CFacade::LoadFromFile( const char *pFileName )
{
   std::ifstream IfStream( pFileName );
   CMinimalHypothesisTree::TMorphoCreator::SmartPtr Ptr;
   std::string CurStr, PrevStr;
   TIDType IDType, PrevIDType;
   std::basic_string<CMinimalHypothesisTree::TChar> WStr;

   IfStream >> CurStr >> IDType;

   while( IfStream )
   {
      Ptr = CMinimalHypothesisTree::TMorphoCreator::Create();
      
      do 
      {
         PrevStr = CurStr;
         PrevIDType = IDType;
         
         if( CanAddIDType(PrevIDType) )
            Ptr->m_IDTypes.insert(PrevIDType);

         IfStream >> CurStr >> IDType;
      }
      while( IfStream && PrevStr == CurStr );

      if( !Ptr->m_IDTypes.empty() )
      {
         Convert(PrevStr, WStr );
         APL_CHECK( m_MinimalHypothesisTree.AddString(WStr.begin(), WStr.end(), Ptr) );
      }
   }
}
///////////////////////////////////////////////////////////////////////////////

bool CFacade::AddStringReverse( const std::wstring &Str, const TTypesList &TypesList )
{
   CMinimalHypothesisTree::TMorphoCreator::SmartPtr Ptr( CMinimalHypothesisTree::TMorphoCreator::Create() );

   for( TTypesList::const_iterator I = TypesList.begin(); I != TypesList.end(); ++I )
      if( CanAddIDType(*I) )
         Ptr->m_IDTypes.insert(*I);


   if( Ptr->m_IDTypes.empty() )
      return false;

   return m_MinimalHypothesisTree.AddString(Str.begin(), Str.end(), Ptr);
}
///////////////////////////////////////////////////////////////////////////////

bool CFacade::TestTree( const char *pFileName ) const
{
   std::ifstream IfStream( pFileName );
   CMinimalHypothesisTree::TMorphoCreator::SmartPtr Ptr = CMinimalHypothesisTree::TMorphoCreator::Create();
   CMinimalHypothesisTree::TMorphoCreator::ConstSmartPtr PtrCmp;
   std::string CurStr, PrevStr;
   TIDType IDType, PrevIDType;
   std::string WStr;

   IfStream >> CurStr >> IDType;

   while( IfStream )
   {
      Ptr->m_IDTypes.clear();

      do 
      {
         PrevStr = CurStr;
         PrevIDType = IDType;

         Ptr->m_IDTypes.insert(PrevIDType);

         IfStream >> CurStr >> IDType;
      }
      while( IfStream && PrevStr == CurStr );

      Convert(PrevStr, WStr );
      
      PtrCmp = m_MinimalHypothesisTree.GetMorpho( WStr.begin(), WStr.end() );

      if( PtrCmp == 0 || Ptr == 0 || *PtrCmp != *Ptr )
         return false;  
   }

   return true;
}
///////////////////////////////////////////////////////////////////////////////


void CFacade::SaveToFileOriginal( const char *pFileName ) const
{
   std::ofstream OfStream( pFileName );
   CMinimalHypothesisTree::SCursor Cursor;
   std::string Str;
   
   for( m_MinimalHypothesisTree.ScanInit( Cursor ); !Cursor.End(); m_MinimalHypothesisTree.ScanNext( Cursor ) )
   {
      APL_ASSERT( Cursor.GetPtr() != 0 );
      APL_ASSERT( !Cursor.GetWord().empty() );
      
      for( 
         CMinimalHypothesisTree::SMorhpo::TIDTypeStotage::const_iterator I = Cursor.GetPtr()->m_IDTypes.begin();
         I != Cursor.GetPtr()->m_IDTypes.end();
         ++I
      )
      {
         OfStream << Convert(Cursor.GetWord(), Str) << '\t' << *I << std::endl;
      }
   }
}
///////////////////////////////////////////////////////////////////////////////

void CFacade::SaveToFileStrict( const char *pFileName, size_t &TotalWordCount, size_t &TotalCharCount ) const
{
   std::ofstream OfStream( pFileName );
   CMinimalHypothesisTree::SCursor Cursor;
   std::string Str;

   TotalWordCount = 0;
   TotalCharCount = 0;

   for( m_MinimalHypothesisTree.ScanInit( Cursor ); !Cursor.End(); m_MinimalHypothesisTree.ScanNext( Cursor ) )
   {
      APL_ASSERT( Cursor.GetPtr() != 0 );
      APL_ASSERT( !Cursor.GetWord().empty() );

      ++TotalWordCount;
      TotalCharCount += Cursor.GetWord().length();

      OfStream << Convert(Cursor.GetWord(), Str) << '\t';

      for( 
         CMinimalHypothesisTree::SMorhpo::TIDTypeStotage::const_iterator I = Cursor.GetPtr()->m_IDTypes.begin();
         I != Cursor.GetPtr()->m_IDTypes.end();
      ++I
         )
      {
         OfStream << ' ' << *I; 
      }

      OfStream << std::endl;
   }
}
///////////////////////////////////////////////////////////////////////////////

void CFacade::SaveToTable( DBMS::IDataTable *pTable ) const
{
   CMinimalHypothesisTree::SCursor Cursor;
   
   std::wstring dbWord;
   INT32 dbIDType = 0;

   void *RecMap[2] = {&dbIDType, &dbWord};

   if( pTable->AddInit(RecMap) != S_OK )   
      SS_THROW( L"Ошибка при инициализации добавления в таблицу минимального дерева поиска" );

   for( m_MinimalHypothesisTree.ScanInit( Cursor ); !Cursor.End(); m_MinimalHypothesisTree.ScanNext( Cursor ) )
   {
      APL_ASSERT( Cursor.GetPtr() != 0 );
      APL_ASSERT( !Cursor.GetWord().empty() );

      dbWord.assign( Cursor.GetWord().rbegin(), Cursor.GetWord().rend() );

      for( 
         CMinimalHypothesisTree::SMorhpo::TIDTypeStotage::const_iterator I = Cursor.GetPtr()->m_IDTypes.begin();
         I != Cursor.GetPtr()->m_IDTypes.end();
      ++I
         )
      {
         dbIDType = *I;
        
         DEBUG_MSG_LOG5( Convert(dbWord, std::string()) << "\t" << dbIDType );

         if( pTable->Add() != S_OK )
            SS_THROW( L"Ошибка при добавлении в таблицу минимального дерева поиска" );
      }
   }
}
///////////////////////////////////////////////////////////////////////////////
// class CMinimalHypothesisTreeBuilder
///////////////////////////////////////////////////////////////////////////////
bool CMinimalHypothesisTreeBuilder::Init( TDictNumber DictNumber )
{
   SS_TRY
   {
      APL_ASSERT_PTR( GetLoadManager() );
      
      SS::Interface::Core::ResourceManagers::ILingvoBaseManager* pLingvoBaseManager = 
         (SS::Interface::Core::ResourceManagers::ILingvoBaseManager*)
         GetLoadManager()->GetInterface(L"./core/linguistics/LingvoBaseManager.dll", CLSID_LinguoBaseManager, IID_LingvoBaseManager);

      if( pLingvoBaseManager == 0 )
         SS_THROW( L"Ошибка при получении LingvoBaseManager" );

      m_DBMSPtrsManager.SetLingvoBaseManager( pLingvoBaseManager );
      m_DBMSPtrsManager.Clear();

      IAMConverterMorpho  *pAMConverter = (IAMConverterMorpho*)
         GetLoadManager()->GetInterface(L"./core/linguistics/AMConverter.dll", CLSID_AMConverterMorpho, IID_AMConverterMorpho);

      if( pAMConverter == 0 )
         SS_THROW( L"Ошибка при получении AMConverterMorpho" );

      switch(DictNumber)
      {
      case DATA_TYPE::NAMES::ednENominalName:
         m_DBMSPtrsManager.Register( SS::MANAGER::Constants::c_szMorphoEng, &m_pMorphoDB );
         break;

      case DATA_TYPE::NAMES::ednRNominalName:
         m_DBMSPtrsManager.Register( SS::MANAGER::Constants::c_szMorphoRus, &m_pMorphoDB );
         break;

      default:
         APL_THROW( _T("По DictNumber не удалось определить язык, DictNumber = ") << (int)DictNumber );
      };

      m_DBMSPtrsManager.Open();

      m_Facade.Init( pAMConverter, m_pMorphoDB, DictNumber );
   }
   SS_CATCH( L"" )

   return false;
}
///////////////////////////////////////////////////////////////////////////////

bool CMinimalHypothesisTreeBuilder::AddReverseWord( const std::wstring* pwReverseWord, const TTypesList* pTypesList )
{
   APL_ASSERT( pwReverseWord );
   APL_ASSERT( pTypesList );

   SS_TRY
   {
      return m_Facade.AddStringReverse( *pwReverseWord, *pTypesList );
   }
   SS_CATCH( L"" )

   return false;
}
///////////////////////////////////////////////////////////////////////////////

void CMinimalHypothesisTreeBuilder::Build()
{
   SS_TRY
   {
      m_Facade.Optimize();

      CTablePtr pTable;
      
      if( pTable.Open(m_pMorphoDB, L"NewWordsDeduction") != S_OK )
         SS_THROW( L"Ошибка при открытии таблицы NewWordsDeduction" );

      pTable->Clear();

      m_Facade.SaveToTable( pTable.Get() );
   }
   SS_CATCH( L"" )
}


} //namespace MinimalHypothesisTree
} //namespace Dictionary
} //namespace SS

