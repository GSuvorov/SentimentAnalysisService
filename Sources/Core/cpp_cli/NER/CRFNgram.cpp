#include "CRFNgram.h"

using System::Collections::Generic::IEnumerable;
using System::Collections::Generic::List;
using System::Collections::Generic::Dictionary;

// Получить части N-граммы
IEnumerable<NER::CRFAttribute^>^ NER::Ngram::GetParts()
{
	return _parts;
}

NER::Ngram::Ngram(IEnumerable<NER::CRFAttribute^>^ parts)
{
	_parts = parts;
}

