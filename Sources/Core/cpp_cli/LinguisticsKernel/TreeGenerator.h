#pragma once

#include "..\..\cpp\ASSInterface\IBlackBox.h"
#include "..\..\cpp\ASSInterface\IAmconverter.h"
#include "StringService.h"
namespace LinguisticsKernel
{
	public ref class CTreeGenerator
	{
		SS::Interface::Core::AMConverter::IAMConverterMorpho* m_pAMConverter;
	public:
		CTreeGenerator(SS::Interface::Core::ResourceManagers::ILoadManager* pLoadManager);
		~CTreeGenerator(void);

		///отрисует дерево по IText
		void GenerateTree(SS::Interface::Core::BlackBox::IText* pText, System::Windows::Forms::TreeView^ pTree, bool bIsSetObligatory);
		///отрисует дерево по IQuery
		void GenerateTree(SS::Interface::Core::BlackBox::Query::IQuery* pQuery, System::Windows::Forms::TreeView^ pTree);

		void GenerateTree1(SS::Interface::Core::BlackBox::IText* pText, System::Windows::Forms::TreeView^ pTree, bool bIsSetObligatory);

	protected:
		CStringService^ m_pStringService;
		///отрисует юнит
		void ShowUnit(SS::Interface::Core::BlackBox::IUnit* pUnit, System::Windows::Forms::TreeNode^ pTNUnit, System::UInt32 nUnit, bool bIsSetObligatory);
	};
}