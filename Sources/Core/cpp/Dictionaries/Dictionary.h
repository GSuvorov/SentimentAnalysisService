#pragma once

#include ".\AbbreviationSource.h"

namespace SS{ 
namespace Dictionary{

//Заглушка для ILoad
class CLoadStub: public ILoad
{
public: 
   /// сохраняет в бинарный вид структуры данных 
   bool ToBinary() { return true; }
   
   /// перезагружает словарь
   bool ReLoad() { return true; }
   
   /// загружает словарь
   bool Load() { return true; }
   
   /// освобождает память
   bool Clear() { return true; }
};
                                                           
class CDictionary:
   public IDictionary, 
   private SS::Interface::Core::Common::IConnection //Управляе соединенем с базой новых слов
{
	CDBMSPtrsManager m_DBMSPtrsManager;
	CDBMSPtrsManager m_NewWordsDBMSPtrsManager;

	Edit::CSourceAdapter m_SourceAdapter;
	Edit::CAbbreviationSourceAdatper m_AbbreviationSourceAdatper;
	Edit::COneRootAdapter m_OneRootAdapter;
	Edit::CMorphoTypesCollectionAdapter m_MorphoTypesCollectionAdapter;

	CNewWordsList  m_NewWordsList;
	CMALanguagesManager m_MALanguagesManager;

	DBMS::IDataBase *m_pMorphoEngDB;
	DBMS::IDataBase *m_pMorphoRusDB;
	DBMS::IDataBase *m_pAbbreviationEngDB;
	DBMS::IDataBase *m_pAbbreviationRusDB;
	DBMS::IDataBase *m_pNdxDB;


private:
   //Иницализировать все внутренние объекты (Единственная функция которую необходимо изменять при 
   //изменении количества словарей)
   void InitObjects( SS::Interface::Core::ResourceManagers::ILoadManager* pLoadManager );


private:
   ///////////////////////////////////////////////////////////////////////////////
   // IConnection
   bool Open();
   bool Close();

public:
	CDictionary();

	/// возвращает интерфейс работы с однокоренными словами
	ILoad* GetLoader();

	/// возвращает интерфейс работы с однокоренными словами
	IOneRoot* GetOneRoot();

	/// возвращает интерфейс работы с типами морфообразования
	IMorphoTypesCollection* GetMorphoTypesCollection();

	/// возвращает интерфейс работы с основами
	ISource* GetSource();

   /// возвращает интерфейс получения и редактирования информации об основах слов
   /// словарей аббревиатур.
   /// Слово может храниться под одним и тем же идентификатором как в базовом словаре 
   /// так и в словаре аббревиатур, поэтому при удалении необходимо использовать
   /// оба интерфейса.
   ISourceEx* GetAbbreviationSource();

	/// возвращает интерфейс морфологического анализа
	IMorphologyAnalyser* GetMorphologyAnalyser();

   /// возвращает интерфейс для получения списков слов определенного типа,
   /// где eDictionariesNumber - номер словаря, для которого необходимо вывести списки слов
   IWordsList* GetWordList();

   void SetLoadManager(SS::Interface::Core::ResourceManagers::ILoadManager* pLoadManager);

   void SetAnalyseParams( SS::Interface::Core::MainAnalyse::IAnalyseParams* pAnalyseParams );

public:
   RELEASE_IMPL()

	BEGIN_QI_MAP()
		QI_MAP_INHERIT_ENTRY( IID_Base, IBase )
		QI_MAP_INHERIT_ENTRY( IID_IDictionary, IDictionary )
	END_QI_MAP()
};

} //namespace Dictionary
} //namespace SS
