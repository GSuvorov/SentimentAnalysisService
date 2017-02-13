#pragma once


namespace SS
{
	namespace Interface
	{
		namespace Core
		{
			namespace BlackBox
			{
/// инкапсулятор морфологических и иных статических характеристик
/**
	инкапсулятор морфологических и иных статических характеристик
	характерных для отдельной морфоформы
*/
				class IFeature
				{
				public:
					///вернет Индекс морфоформы m_ulIndex
					virtual unsigned int GetMorphoIndex() = 0;
					///установит переданный Индекс морфоформы m_ulIndex
					virtual void SetMorphoIndex(unsigned int uiValue) = 0;
					///вернет  часть морфологической информации m_ulMorphoInfo
					virtual unsigned int GetMorphoInfo() = 0;
					///установит переданную часть морфологической информации m_ulMorphoInfo
					virtual void SetMorphoInfo(unsigned int uiValue) = 0;
					///вернет  часть морфологической информации m_ulPartOfSpeech
					virtual unsigned int GetPartOfSpeech() = 0;
					///установит переданную часть морфологической информации m_ulPartOfSpeech
					virtual void SetPartOfSpeech(unsigned int uiValue) = 0;

					///удаление характеристики
					virtual IFeature* ReleaseFeature() = 0;
					///очистка значений характеристики
					virtual void ClearFeature() = 0;
				};
			}
		}
	}
}
