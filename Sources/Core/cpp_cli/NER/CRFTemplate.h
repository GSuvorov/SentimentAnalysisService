#pragma once
#include "CRFNgram.h"

namespace NER
{
    /// Внутреннее представление шаблона для построения входных данных SRFSuitNER
	ref class CRFTemplate
	{
	private:
		array<System::String^>^ _columnNames; // Наименования столбцов преобразованного входного файла
        List<NER::Ngram^>^ _nGramTemplates; // шаблоны N-грамм

	public:
		///Конструктор шаблона для построения входных данных SRFSuitNER
		///@param columnNames - Наименования столбцов преобразованного входного файла
		///@param nGramTemplates - шаблоны N-грамм
		CRFTemplate(array<System::String^>^ columnNames, List<NER::Ngram^>^ nGramTemplates);

		/// Получить наименования столбцов преобразованного входного файла
		array<System::String^>^ GetColumnNames();

		/// Получить шаблоны N-грамм
		IEnumerable<NER::Ngram^>^ GetNgramTemplates();
	};
}



