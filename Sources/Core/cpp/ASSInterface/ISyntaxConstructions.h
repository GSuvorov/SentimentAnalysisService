#pragma once

#include "../ASCInterface/IInterface.h"
#include "./ICommon.h"
#include "./TSyntaxConstructions.h"
#include "./construction.h"
#include "../ASSInterface/TMainAnalyseTypes.h"

// {7D880849-ECC4-4714-8CDE-BD9540B97D8F}
static const GUID CLSID_ISyntaxConstructionManagerEng = 
{ 0x7d880849, 0xecc4, 0x4714, { 0x8c, 0xde, 0xbd, 0x95, 0x40, 0xb9, 0x7d, 0x8f } };

// {4F31224A-05DF-4673-B11F-E5F598254B59}
static const GUID CLSID_ISyntaxConstructionManagerRus = 
{ 0x4f31224a, 0x5df, 0x4673, { 0xb1, 0x1f, 0xe5, 0xf5, 0x98, 0x25, 0x4b, 0x59 } };

// {98B66331-8DAA-468e-B92A-717E3FF5436A}
static const GUID CLSID_ISyntaxConstructionManagerRus_NQ = 
{ 0x98b66331, 0x8daa, 0x468e, { 0xb9, 0x2a, 0x71, 0x7e, 0x3f, 0xf5, 0x43, 0x6a } };

// {84785B3C-4E85-452d-BE1A-E828EC181545}
static const GUID CLSID_ISemanticConstructionManagerEng = 
{ 0x84785b3c, 0x4e85, 0x452d, { 0xbe, 0x1a, 0xe8, 0x28, 0xec, 0x18, 0x15, 0x45 } };

// {8946F261-F588-4314-AC44-3EA20B8002BF}
static const GUID CLSID_ISemanticConstructionManagerRus = 
{ 0x8946f261, 0xf588, 0x4314, { 0xac, 0x44, 0x3e, 0xa2, 0xb, 0x80, 0x2, 0xbf } };

// {D844B451-E20A-47e3-9D11-FC2B441EFB14}
static const GUID IID_ISyntaxConstructionManager = 
{ 0xd844b451, 0xe20a, 0x47e3, { 0x9d, 0x11, 0xfc, 0x2b, 0x44, 0x1e, 0xfb, 0x14 } };

// {F8EB85AB-69B3-49f0-BF21-482F30EEFC5D}
static const GUID IID_ISyntaxConstructionManagerSQL = 
{ 0xf8eb85ab, 0x69b3, 0x49f0, { 0xbf, 0x21, 0x48, 0x2f, 0x30, 0xee, 0xfc, 0x5d } };


namespace SS
{
	namespace Interface
	{
		namespace Core
		{
			namespace SyntaxConstructions
			{
/**************************************************************************************************************/
				/// интерфейс, предоставляющий набор методов по работе с группами синтаксических конструкций и их названиями
				/**	позволяет создавать, редактировать и удалять названия и номера групп	*/
				class ISyntaxConstructionGroupsNames
				{
				public:
					/// конструктор
					ISyntaxConstructionGroupsNames(void){};
					/// деструктор
					virtual ~ISyntaxConstructionGroupsNames(){};
				
					/// Возвращает список всех номеров конструкций и их имена
					virtual void Get(/*out*/std::list<SS::SyntaxConstructions::Types::SSyntaxConstructionGroupName> * p_list_SyntaxConstructionGroup) = 0;
					/// удаляет текущую конструкцию
					virtual void Delete() = 0;
					/// добаляет текущую конструкцию
					virtual void Add() = 0;
					/// обновляет текущую конструкцию
					virtual void Update() = 0;
					/// устанавливает текущую конструкцию и ее имя
					virtual void Set(/*in*/SS::SyntaxConstructions::Types::SSyntaxConstructionGroupName & oSyntaxConstructionGroupName)
					{
						m_SyntaxConstructionGroupName = oSyntaxConstructionGroupName;
					}
					/// устанавливает текущую конструкцию и ее имя
					virtual void Get(SS::SyntaxConstructions::Types::SSyntaxConstructionGroupName & oSyntaxConstructionGroupName)
					{
						oSyntaxConstructionGroupName = m_SyntaxConstructionGroupName;
					}
				protected:
					///Cтруктура, которая содержит данные о номере конструкции и ее имени
					SS::SyntaxConstructions::Types::SSyntaxConstructionGroupName m_SyntaxConstructionGroupName;
				};
/**************************************************************************************************************/
				/// интерфейс для работы с группой слов для детектирования
				class IDetectWordsGroups
				{
				public:
					/// конструктор
					IDetectWordsGroups(){};
					/// деструктор
					virtual ~IDetectWordsGroups(){};
				
					/// возвращает текущую группу детектирования слов
					virtual std::wstring & GetDetectWordsGroupName()
					{
						return m_wsDetectWordsGroupName;
					}
					/// определяет группу детектирования слов для работы
					virtual void SetDetectWordsGroupName(std::wstring & wsDetectWordsGroupName)
					{
						m_wsDetectWordsGroupName = wsDetectWordsGroupName;
					}					
					/// создает группу слов для детектирования
					virtual bool CreateDetectWordsGroup(SS::SyntaxConstructions::Types::TDetectWordsGroup & oDetectGroup) = 0;
					/// удаляет текущую группу слов для детектирования
					virtual void DeleteDetectWordsGroup() = 0;
				
					/// возвращает все элементы группы слов для детектирования
					virtual void GetWords(SS::SyntaxConstructions::Types::TDetectWordsGroup & oDetectGroup) = 0;
					/// удаляет текущий набор элементов из группы слов для детектирования и устанавливает новый
					virtual void SetWords(SS::SyntaxConstructions::Types::TDetectWordsGroup & oDetectGroup) = 0;
				
					/// определяет входит ли слово в группу для детектирования
					virtual bool IsDetectWord(std::string & sDetectWord) = 0;
				
				protected:
					///Имя группы слов для детектирования
					std::wstring m_wsDetectWordsGroupName;
				};

/**************************************************************************************************************/
				/// интерфейс для работы с синтаксическими конструкциями
				/** позволяет создавать, удалять и работать с созданными конструкциями */
				class ISyntaxConstructions
				{	
				public:
					/// конструктор
					ISyntaxConstructions(void) : m_iID(0) {};
					/// деструктор
					virtual ~ISyntaxConstructions(){};
				
					/// добавляет в базу конструкцию и возвращает ее номер
					virtual long Add(/*in*/SS::SyntaxConstructions::Types::CConstructionEx* p_SyntaxConstruction) = 0;
					/// удаляет текущую конструкцию
					virtual void Delete() = 0;
					/// возвращает список всех синтаксических конструкций
					virtual bool Get(/*out*/std::list<SS::SyntaxConstructions::Types::CConstructionEx>* p_list_OfSyntaxConstruction) = 0;
					/// возвращает текущую синтаксическую конструкцию
					virtual bool Get(/*out*/SS::SyntaxConstructions::Types::CConstructionEx* p_SyntaxConstruction) = 0;
					/// устанавливает номер конструкции для работы	
					virtual void Set(/*in*/int iID)
					{
						m_iID = iID;
					}
					
				protected:
					///	текущий индекс конструкции	
					int m_iID;
				};

/**************************************************************************************************************/
			
				/// интерфейс для работы с группами синтаксических конструкций
				/** содержит набор методов по созданию, удалению и работе с группами конструкций */
				class ISyntaxConstructionGroups
				{	
				public:
					/// конструктор
					ISyntaxConstructionGroups(void){};
					/// деструктор
					virtual ~ISyntaxConstructionGroups(){};
									
					/// устанавливает номер конструкции для работы
					virtual void SetConstructionId(/*in*/int iConstructionId)
					{
						m_GroupAndConstruction.m_i32IDConstruction = iConstructionId;
					}
					/// возвращает текущий номер конструкции 
					virtual int  GetConstructionId()
					{
						return m_GroupAndConstruction.m_i32IDConstruction;
					}
					/// устанавливает группу конструкций для работы
					virtual void SetConstructionGroupId(int iConstructionGroupId)
					{
						m_GroupAndConstruction.m_i32IDGroup = iConstructionGroupId;
					}
					/// возвращает текущую группу конструкций
					virtual int GetConstructionGroupId()
					{
						return m_GroupAndConstruction.m_i32IDGroup;
					}
					///	добавляем текущую пару 	(m_iConstructionId, m_ConstructionGroupId) в базу
					virtual bool Add() = 0;	
					/// возвраращает все пары (m_iConstructionId, m_ConstructionGroupId), хранимые в базе
					virtual bool Get(std::list<SS::SyntaxConstructions::Types::SGroupAndConstruction> & listConstructionGroups) = 0;					
					///	удаляет текущую пару из базы
					virtual bool Delete() = 0;
					/// возвращает список структур содержащих группу для типа разбора и принадлежность к гр. разр. омонимии
					virtual void GetGroupsForType( SS::Core::Types::EAnalyseMods eAnalyseMode, 
							std::list<SS::SyntaxConstructions::Types::SGroupAndCharacteristics> & lstGroupCharacteristics) = 0;
				
				protected:
					///	соответствие конструкции и группы, к которой она относится
					SS::SyntaxConstructions::Types::SGroupAndConstruction m_GroupAndConstruction;
				};

/**************************************************************************************************************/

				/// интерфейс, управляющий работой кобинаторно синтактико семантического словаря
				class ISyntaxConstructionsManager : public virtual SS::Interface::IBase,
													public virtual SS::Interface::Core::Common::ILoad
				{
				public:
					/// конструктор
					ISyntaxConstructionsManager(void){};
					/// деструктор
					virtual ~ISyntaxConstructionsManager(){};
				public:
					///	возвращает интерфес для работы с синтаксическими конструкциями	
					virtual SS::Interface::Core::SyntaxConstructions::ISyntaxConstructions			* GetSyntaxConstructions() = 0;
					///	возвращает интерфес для работы с группами синтаксических конструкций	
					virtual SS::Interface::Core::SyntaxConstructions::ISyntaxConstructionGroups		* GetSyntaxConstructionGroups() = 0;
					///	возвращает интерфес для работы с группами синтаксических конструкций и их названиями	
					virtual SS::Interface::Core::SyntaxConstructions::ISyntaxConstructionGroupsNames	* GetSyntaxConstructionGroupsNames() = 0;
					///	возвращает интерфес для работы с группами слов для детектирования	
					virtual SS::Interface::Core::SyntaxConstructions::IDetectWordsGroups				* GetDetectWordsGroups() = 0;
				};

			}
		};
	};
};