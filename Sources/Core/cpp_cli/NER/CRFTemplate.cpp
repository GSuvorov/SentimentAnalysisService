#include "CRFTemplate.h"

using System::Collections::Generic::IEnumerable;
using System::Collections::Generic::List;
using System::Collections::Generic::Dictionary;

NER::CRFTemplate::CRFTemplate(array<System::String^>^ columnNames, List<NER::Ngram^>^ nGramTemplates)
{
    _columnNames = columnNames;
    _nGramTemplates = nGramTemplates;
}

// Получить наименования столбцов преобразованного входного файла
array<System::String^>^ NER::CRFTemplate::GetColumnNames()
{
    return _columnNames;
}

// Получить шаблоны N-грамм
IEnumerable<NER::Ngram^>^ NER::CRFTemplate::GetNgramTemplates()
{
    return _nGramTemplates; 
}
