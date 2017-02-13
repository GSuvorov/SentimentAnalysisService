#include "StdAfx.h"
#include "NERProcessor.h"
#include <list>

using System::Collections::Generic::List;
using SS::Interface::Core::BlackBox::IUnit;
using namespace NER;

SS::LinguisticProcessor::NERProcessor::NERProcessor(System::String^ path)
{
	_unitTable = gcnew System::Collections::Generic::List<UnitTableItem^>();
	_pCombinatoryUnits = new std::vector<SS::Interface::Core::BlackBox::IUnit*>();
	m_RecognizerRUS = gcnew NamedEntityRecognizer(System::IO::Path::Combine(path, L"models", L"model_ru"),
												  System::IO::Path::Combine(path, L"templateNER.txt"));
	m_RecognizerENG = gcnew NER::NamedEntityRecognizer(System::IO::Path::Combine(path, L"models", L"model_en"),
		                                               System::IO::Path::Combine(path, L"templateNER.txt"));
}

SS::LinguisticProcessor::NERProcessor::~NERProcessor()
{
	if (_pCombinatoryUnits)
	{
		delete _pCombinatoryUnits;
		_pCombinatoryUnits = 0;
	}
}

/// const wstring& plainText передается лишь для того, чтобы " - " не склеивать с предыдущим словом 
void SS::LinguisticProcessor::NERProcessor::Run(
	SS::Interface::Core::BlackBox::IText* pText,
	const wstring& plainText,
	SS::Interface::Core::LexicalAnalyze::ILexica* pLexica)
{
	for (SS::Interface::Core::BlackBox::IParagraph *pPar = pText->GetFirstParagraph(); pPar; pPar = pPar->GetNextParagraph())
	{
		for (SS::Interface::Core::BlackBox::ISentence *pSen = pPar->GetFirstSentence(); pSen; pSen = pSen->GetNextSentence())
		{
			RunSentence(GetRecognizer(pSen), pSen, plainText, pLexica);
		}
	}
}

NER::NamedEntityRecognizer^ SS::LinguisticProcessor::NERProcessor::GetRecognizer(SS::Interface::Core::BlackBox::ISentence* pSentence)
{
	if (pSentence->GetLanguage() == SS::Core::Types::EAnalyseLanguage::ealRussian)
	{
		return m_RecognizerRUS;
	}
	else
	{
		return m_RecognizerENG;
	}
}

void SS::LinguisticProcessor::NERProcessor::RunSentence(
	NER::NamedEntityRecognizer^ pRecognizer,
	SS::Interface::Core::BlackBox::ISentence* pSentence,
	const wstring& plainText,
	SS::Interface::Core::LexicalAnalyze::ILexica* pLexica)
{
	System::String^ preparedString = PrepareSentence(pSentence, plainText);
	SaveNamedEntitiesSemantic(pRecognizer->Run(preparedString), pLexica);
	SetSemanticForCombinatoryUnits(pLexica);
	_unitTable->Clear();
	_pCombinatoryUnits->clear();
}

void SS::LinguisticProcessor::NERProcessor::SetSemanticForCombinatoryUnits(SS::Interface::Core::LexicalAnalyze::ILexica* pLexica)
{
	for (vector<SS::Interface::Core::BlackBox::IUnit*>::iterator it = _pCombinatoryUnits->begin();
		 it != _pCombinatoryUnits->end();
		 ++it)
	{
		SetSemanticForCombinatoryUnit(*it, pLexica);
	}
}

void SS::LinguisticProcessor::NERProcessor::SetSemanticForCombinatoryUnit(
		SS::Interface::Core::BlackBox::IUnit* pParentUnit,
		SS::Interface::Core::LexicalAnalyze::ILexica* pLexica)
{
	bool isAnyProduct = false;
	std::list<SS::Interface::Core::BlackBox::IUnit*> list = pParentUnit->GetChildList();
	for	(std::list<SS::Interface::Core::BlackBox::IUnit*>::iterator it = list.begin(); it != list.end(); ++it)
	{
		SS::Interface::Core::BlackBox::IUnit *pUnit = *it;
		if (pLexica->IsSemanticType(pUnit, SS::Core::Features::Values::TSemanticType.smtProduct))
		{
			isAnyProduct = true;
			break;
		}
	}
	if (isAnyProduct)
	{
		pLexica->SetSemanticType(pParentUnit, SS::Core::Features::Values::TSemanticType.smtProduct);
	}
}

void SS::LinguisticProcessor::NERProcessor::SaveNamedEntitiesSemantic(
		System::Collections::Generic::List<System::String^>^ semantic,
		SS::Interface::Core::LexicalAnalyze::ILexica* pLexica)
{
	int previousLineIndex = -1;
	TUnitList combinatory;
	for (int i = 0; i < _unitTable->Count; ++i)
	{
		UnitTableItem^ item = _unitTable[i];
		int lineIndex = item->GetLineIndex();
		SS::Interface::Core::BlackBox::IUnit* pUnit = item->GetUnit();
		System::String^ semanticType = semantic[lineIndex];
		if (!IsCombiPart(semanticType))
		{
			/*if (combinatory.size() > 1)
			{
			}*/
			combinatory.clear();
		}
		if (IsNamed(pUnit))
		{
			SS::Core::Features::Types::TSemanticType semType = GetSemantic(semanticType);
			bool isFirstSentenceWord = (lineIndex == 0) || (lineIndex - previousLineIndex > 1);
			if (!semType.IsUndefined() || !isFirstSentenceWord)
			{
				combinatory.push_back(pUnit);
				pLexica->SetSemanticType(pUnit, semType);
				SS::Interface::Core::BlackBox::IUnit* pParentUnit = pUnit->GetParentUnit();
				if (pParentUnit)
				{
					pLexica->SetSemanticType(pParentUnit, semType);
				}
			}
		}
		previousLineIndex = lineIndex;
	}
}

bool SS::LinguisticProcessor::NERProcessor::IsCombiPart(System::String^ semantic)
{
	return semantic->StartsWith("I-");
}

SS::Core::Features::Types::TSemanticType SS::LinguisticProcessor::NERProcessor::GetSemantic(System::String^ semantic)
{
	SS::Core::Features::Types::TSemanticType result = SS::Core::Features::Values::TSemanticType.Undefined;
	if (semantic->Equals("B-ORG") || semantic->Equals("I-ORG"))
		result = SS::Core::Features::Values::TSemanticType.smtOrganization;
	else if (semantic->Equals("B-NAME") || semantic->Equals("I-NAME"))
		result = SS::Core::Features::Values::TSemanticType.smtPerson;
	else if (semantic->Equals("B-GEO") || semantic->Equals("I-GEO"))
		result = SS::Core::Features::Values::TSemanticType.smtPlace;
	else if (semantic->Equals("B-ENTR") || semantic->Equals("I-ENTR"))
		result = SS::Core::Features::Values::TSemanticType.smtEvent;
	else if (semantic->Equals("B-PROD") || semantic->Equals("I-PROD"))
		result = SS::Core::Features::Values::TSemanticType.smtProduct;
	return result;
}

bool SS::LinguisticProcessor::NERProcessor::IsNamed(SS::Interface::Core::BlackBox::IUnit* pUnit)
{
	if (pUnit)
	{
		wstring word = pUnit->GetWord();
		for (int i = 0; i < word.size(); ++i)
		{
			if (iswupper(word[i]))
			{
				return true;
			}
		}
	}
	return false;
}

/// подготовка данных для дальнейшей обработки в CRFsuit
System::String^ SS::LinguisticProcessor::NERProcessor::PrepareSentence(
	SS::Interface::Core::BlackBox::ISentence* pSentence,
	const wstring& plainText)
{
	System::Text::StringBuilder^ result = gcnew System::Text::StringBuilder();
	int stringNumber = 0;
	TUnitList sentUnits;
	pSentence->GetLinearUnitsFull(sentUnits);
	TUnitList::iterator prevLastIt = GetIteratorAfterLastWord(sentUnits);
	PrepareMainPart(sentUnits, prevLastIt, result, plainText, stringNumber);
	PrepareEndPart(sentUnits, prevLastIt, result, stringNumber);
	return result->ToString()->Trim();
}

/// Получить итератор после указывающий на позицию, следующую за последним словом
/// Справа от него останутся многоточия (и знаки пунктуации в конце предложения)
TUnitList::iterator SS::LinguisticProcessor::NERProcessor::GetIteratorAfterLastWord(TUnitList& sentUnits)
{
	TUnitList::reverse_iterator ri = sentUnits.rbegin();
	for (; ri != sentUnits.rend(); ++ri)
	{
		IUnit *pUnit = *ri;
		wstring word = pUnit->GetWord();
		if (word.compare(L"."))
		{
			break;
		}
	}
	return TUnitList::iterator(ri.base());
}

/// Подготовка основной части предложения
/// Всё предложение, кроме многоточия (и знаков пунктуации в конце предложения)
void SS::LinguisticProcessor::NERProcessor::PrepareMainPart(
	TUnitList& sentUnits,
	TUnitList::iterator& prevLastIt,
	System::Text::StringBuilder^ stringBuilder,
	const wstring& plainText,
	int& stringNumber)
{
	IUnit *prevUnit = 0;
	for (TUnitList::iterator itU = sentUnits.begin(); itU != prevLastIt; ++itU)
	{
		IUnit *pUnit = *itU;

		wstring token = pUnit->GetWord();
		if (!token.empty() && pUnit->GetChildList().empty())
		{
			AppendUnitStringToLine(prevUnit, pUnit, stringBuilder, plainText, stringNumber);
		}
		else
		{
			_pCombinatoryUnits->push_back(pUnit);
		}
	}
}

/// Обработка окончания предложения
/// Обработка многоточий (и знаков пунктуации) в конце предложения 
void SS::LinguisticProcessor::NERProcessor::PrepareEndPart(
	TUnitList& sentUnits,
	TUnitList::iterator& prevLastIt,
	System::Text::StringBuilder^ stringBuilder,
	int& stringNumber)
{
	wstring concatWord = L"";
	for (TUnitList::iterator itU = prevLastIt; itU != sentUnits.end(); ++itU)
	{
		IUnit *pUnit = *itU;
		wstring word = pUnit->GetWord();
		concatWord += word;

		_unitTable->Add(gcnew UnitTableItem(pUnit, stringNumber));
	}
	stringBuilder->AppendLine( gcnew System::String(concatWord.c_str()) );

	++stringNumber;
}

/// Сформировать строку на основании рассматрения pUnit и prevUnit
void SS::LinguisticProcessor::NERProcessor::AppendUnitStringToLine(
	IUnit* &prevUnit,
	IUnit* pUnit,
	System::Text::StringBuilder^ stringBuilder,
	const wstring& plainText, 
	int& stringNumber)
{
	if (prevUnit)
	{
		bool isMerged = MergeWithPrev(prevUnit, pUnit, stringBuilder, plainText);
		if (isMerged)
		{
			--stringNumber;
		}
	}

	wstring token = pUnit->GetWord();
	if (token == L"–")
	{
		int position = pUnit->GetFirstCharPositionNew();
		char symbol = plainText[position];
		if (symbol == L' ') /// дефис
		{
			++position;
		}
		stringBuilder->Append(plainText[position]);
		stringBuilder->AppendLine();
	}
	else
	{
		stringBuilder->AppendLine( gcnew System::String(token.c_str()) );
	}
	prevUnit = pUnit;

	_unitTable->Add(gcnew UnitTableItem(pUnit, stringNumber));
	++stringNumber;
}

/// Запятая является разделителем? (или это часть юнита, например -> [123,432])
bool SS::LinguisticProcessor::NERProcessor::IsCommaSeparator(wstring token)
{
	int result;
	return !System::Int32::TryParse( gcnew System::String(token.c_str()), result );
}

/// Объединение текущего и предыдущего юнитов
bool SS::LinguisticProcessor::NERProcessor::MergeWithPrev(
	IUnit* &prevUnit, 
	IUnit *pUnit, 
	System::Text::StringBuilder^ stringBuilder,
	const wstring& plainText)
{
	wstring token = pUnit->GetWord();
	wstring prevToken = prevUnit->GetWord();

	int pUnitPosition = pUnit->GetFirstCharPositionNew();
	int prevLastPozition = prevUnit->GetFirstCharPositionNew() + prevToken.size();

	if ((token == L"–") && plainText[pUnitPosition] != L'–')
		return false;

	if ((token.compare(L",") == 0) && IsCommaSeparator(prevToken))
		return false;

	if (IsPunctuation(prevToken) || IsPunctuation(token))
		return false;
	
	if (pUnitPosition == prevLastPozition)
	{
		int length = System::Environment::NewLine->Length;
		stringBuilder->Remove(stringBuilder->Length - length, length);
		return true;
	}
	
	return false;
}

/// Символ пунктуации?
bool SS::LinguisticProcessor::NERProcessor::IsPunctuation(wstring token)
{
	if ((token == L"!") || (token == L"?") || (token == L":") || (token == L"\"") || (token == L"«") || (token == L"»") ||
		(token == L"“") || (token == L"”"))
	{
		return true;
	}
	return false;
}