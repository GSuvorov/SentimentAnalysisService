#pragma once
#include "..\..\cpp\ASSInterface\IBlackBox.h"

#include "..\..\cpp\ASSInterface\ICoSeDi.h"
namespace LinguisticsKernel
{
	public ref class CStringService
	{
	public:
		CStringService(void);
		~CStringService(void);

		///выдаст стринговое описание типа индекса
		System::String^ IndexTypeToString(SS::Interface::Core::BlackBox::EIndexType indexType);
		///выдаст стринговое описание типа трансформации
		System::String^ TransformationTypeToString(SS::Interface::Core::BlackBox::Query::ETransformationType transformationType);

		///выдаст стринговое описание номера словаря
		System::String^ DictionaryIndexToString(System::UInt32 index);
		///выдаст краткое стринговое описание номера словаря
		System::String^ DictionaryIndexToShortString(System::UInt32 index);

		///выдаст стринговое описание семантического индекса
		System::String^ SemanticIndexToString(System::UInt32 index);
		///выдаст стринговое описание семантического индекса
		System::String^ SyntaxIndexToString(System::UInt32 index);
		///выдаст стринговое описание синтаксического индекса
		System::String^ SynonymIndexToString(System::UInt32 index);
		///выдаст стринговое описание морфологического индекса
		System::String^ MorphoIndexToString(System::UInt32 index);
		///выдаст краткое стринговое описание морфологического индекса
		System::String^ MorphoIndexToShortString(System::UInt32 index);

		System::String^ DictionaryIndexToString1(System::UInt32 index);

		System::String^ LanguageIndexToString(System::UInt32 index);

		//System::String^ GetMorphoString(SS::Core::Features::CMorphoFeature& oMorphoFeature);



//		void SetSemanticClassesTree(SS::Interface::Core::ISemanticClassesTree* pSemanticClassesTree);
	};
}