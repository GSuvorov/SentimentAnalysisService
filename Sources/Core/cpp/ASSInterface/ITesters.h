#pragma once

#include "../ASCInterface/IInterface.h"
#include "./IBlackBoxItems.h"
#include "./Construction.h"

// {b1629ff1-9f26-42bb-b330-8afb33cb8c6d} 
static const GUID CLSID_ConstructionManager = 
{ 0xb1629ff1, 0x9f26, 0x42bb, { 0xb3, 0x30, 0x8a, 0xfb, 0x33, 0xcb, 0x8c, 0x6d } };

// {d83e285f-2106-41e6-8d72-98cb4d51640b} 
static const GUID IID_ConstructionManager = 
{ 0xd83e285f, 0x2106, 0x41e6, { 0x8d, 0x72, 0x98, 0xcb, 0x4d, 0x51, 0x64, 0x0b } };

namespace SS
{
	namespace Interface
	{
		namespace Test
		{
			///Интерфейс контролера экспертов.
			class IConstructionManager :
				virtual public SS::Interface::IBase,
				virtual public SS::Interface::Core::Common::CAnalyseParams
			{
			public:
				typedef std::vector<SS::Interface::Core::BlackBox::IUnit *> TUnitsVector;
            typedef std::vector<TUnitsVector> TUnitsCombination;
	            
				//Юниты в дереве которые удовлетворяют ограничениям текущей конструкции.
				//При этом каждый элемент внешнего вектора представляет собой различные комбинации
				//юнитов подходящих под правило конструкции.
				//Этот тип используется для возможности, не только увидеть результат преобразования, 
				//но и те элементы которые удовлетворяют правилам конструкции.
				typedef std::vector<TUnitsCombination> TDetectedUnits;

			public:
				// Перезагрузка конструкций из базы.
				// Возвр: true - загрузка прошла удачно
				virtual BOOL InitCounstructions() = 0;
				// Сделать Construction текущей конструкцией.(Следующую конструкцию получить невозможно)
				// Возвр: true - загрузка прошла удачно
				virtual BOOL InitCounstructions( const SS::SyntaxConstructions::Types::CConstructionEx& Construction ) = 0;
				
///////////////////////////////////////////////////////////////////////
				/// Устанавливает анализируемое предложение
				virtual void SetText(SS::Interface::Core::BlackBox::IText* pText) = 0;
///////////////////////////////////////////////////////////////////////				
				/// Делает текущей очередную конструкцию; но не применяет ее , вернет true, если текущая конструкция не последняя
				virtual BOOL GetNextCounstruction() = 0;
				/// возвращает информацию о текущей конструкции
				virtual BOOL GetCurrentCounstructionInfo(SS::SyntaxConstructions::Types::CConstructionEx& oConstructionEx) = 0;
				/// применяет очередную конструкцию
				virtual BOOL ApplyCounstruction() = 0;

				/// Делает текущей очередную группу; вернет true, если текущая группа конструкций не последняя
				virtual BOOL GetNextGroup() = 0;
				/// возвращает информацию о текущей группе
				virtual BOOL GetCurrentGroupInfo(std::wstring & wName) = 0;
				/// применяет до конца текущую группу экспертов
				/// если часть экспертов текущей группы выполнено, а часть нет (GetNextCounstruction(), ApplyCounstruction()),
				/// то выполняет текущую группу до конца
				virtual BOOL ApplyGroup() = 0;
///////////////////////////////////////////////////////////////////////
				/// Найти элементы удовлетворяющие правилам текущей конструкции в pText и записать указатели 
				/// на них в DetectedUnits
				virtual void GetDetectedUnits(TDetectedUnits& v_DetectedUnits) = 0;

            //Сохранить данные дерева pText в текстовый файл по пути FileName
            virtual void SaveTreeToFile(SS::Interface::Core::BlackBox::IText* pText, const std::wstring & FileName) = 0;
			};
		}
	}
}