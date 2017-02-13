#pragma	 once

#include "IExpertController.h"
#include "GroupManager.h"
#include "BoardCommonTypes.h"
#include "Board.h"
#include "Block.h"

namespace SS
{
	namespace Syntax
	{
		namespace ExpertModel
		{
			///Управляет последовательностью применения экспертов.
			class CExpertController : public SS::Interface::Syntax::IExpertController
			{
			private:
				///Менеджер групп
				static CGroupManager				m_oGroupManager;
				///Итератор на текущую группу экспертов
				CGroupManager::TCollectionItr		m_itCurrentGroupPtr;
				///Итератор на текущего эксперта
				CExpertGroup::TExpertPtrItr			m_itCurrentExpertPtr;
				///Обрабатываемая доска
				BoardElements::CBoard				m_oBoard;
				///Указывает, произошло ли разбиение на гипотезы
				bool m_isVersioned;
				///флаг для определения прохода по одной гипотезе или по всем
				bool m_bOneHyp;
				///номер гипотезы
				int m_iNumHyp;
				///Показатель успешной инициализации доски
				bool m_bInitBoard;
					
			public:
				///Конструктор
				CExpertController(ESyntaxBase eBase);
				///Возвращает ининтерфейс для работы с загрузкой экспертов
				SS::Interface::Core::Common::ILoad* GetLoader();
				///Переопределяю от IBase
				void SetLoadManager(SS::Interface::Core::ResourceManagers::ILoadManager* pLoadManager);
				///Установка параметров
				void SetParams(bool bOneHyp, int iNumHyp);
				///Устанавливает анализируемое предложение
				virtual void SetSentence(SS::Interface::Core::BlackBox::ISentence* pSentence);
				///Устанавливает анализируемое предложение для тестовых целей
				virtual void SetSentenceTest( SS::Interface::Core::BlackBox::ISentence* pSentence,  
											  SS::Syntax::ExpertModel::ParsingType type );
				///Запускает синтаксический разбор предложения с динамическим созданием гипотез
				virtual void ParseSentenceDynamic(ParsingType pType);
				///Запускает очередного эксперта
				virtual void PerformStep();
				///Прогоняет до конца текущую группу экспертов
				virtual void PerformGroup();
				///Обрабатывает очередную гипотезу
				virtual void PerformVersion();
				///Возвращает обрабатываемую доску
				virtual BoardElements::CBoard & GetBoard()
				{
					return m_oBoard;
				}
				///Возвращает строку, описывающую текущего эксперта
				virtual std::wstring GetCurrentExpertString()
				{
                    std::wstring result = L"";
                    if((m_itCurrentGroupPtr != m_oGroupManager.GetEndGroup()) && 
                       (m_itCurrentExpertPtr != (*m_itCurrentGroupPtr)->end()))
                        result = (*m_itCurrentExpertPtr)->GetName();
					return result;
				}
				///Произошло ли разбиение на гипотезы
				virtual bool IsVersioned()
				{
					return !((*m_itCurrentGroupPtr)->IsAmbiguity());
				}

				///Удаление
				virtual ULONG Release()
				{
					delete this;
					return 0;
				}
				///Запрос интерфейса
				virtual HRESULT QueryInterface(REFIID pIID, void** pBase)
				{
					if (pIID == IID_ExpertController)
					{
						*pBase = static_cast< SS::Interface::Syntax::IExpertController* >( this );
					}
					else if (pIID == IID_Base)
					{
						*pBase = static_cast< SS::Interface::IBase* >( this );
					}
					else
					{
						*pBase = NULL;
					}
					return S_OK;
				}
				///Установить AnalyseParams
				void SetAnalyseParams(SS::Interface::Core::MainAnalyse::IAnalyseParams* pAnalyseParams);
			};
		}
	}
}