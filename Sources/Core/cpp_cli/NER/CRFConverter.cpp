#include "CRFConverter.h"

NER::CRFConverter::CRFConverter(NER::CRFTemplate^ crfTemplate)
{
	m_crfTemplate = crfTemplate;
	_attributeIndexTable = gcnew Dictionary<System::String^, int>();
	InitializeAttributeIndexTable();
	_stringBuilder = gcnew System::Text::StringBuilder();
}

System::String^ NER::CRFConverter::ToCRFFormat(System::Collections::Generic::List<NER::Sentence^>^ sentences)
{
    _stringBuilder->Clear();

    for each (NER::Sentence^ sentence in sentences)
	{
		ConvertSentenceToCRF(sentence);
		_stringBuilder->AppendLine();
	}

    return _stringBuilder->ToString();
}


void NER::CRFConverter::InitializeAttributeIndexTable()
{
    int index = 0;
    for each (System::String^ columnName in m_crfTemplate->GetColumnNames())
    {
        _attributeIndexTable->Add(columnName, index);
        ++index;
    }
}

void NER::CRFConverter::ConvertSentenceToCRF(NER::Sentence^ sentence)
{
	System::Collections::Generic::List<NER::Word^>^ words = sentence->GetWords();
    for (int i = 0; i < words->Count; ++i)
    {
		System::Collections::Generic::List<System::String^>^ characteristics = words[i]->GetGraphematicCharacteristics();
        _stringBuilder->Append(characteristics[characteristics->Count - 1] + "\t");

        for each (Ngram^ ngram in m_crfTemplate->GetNgramTemplates())
            if (CanTemplateBeApplied(sentence, i, ngram->GetParts()))
                ApplyTemplate(sentence, i, ngram->GetParts());

        if (i == 0)
            _stringBuilder->Append(BeginOfSentence);
        else if (i == words->Count - 1)
            _stringBuilder->Append(EndOfSentence);

        _stringBuilder->AppendLine();
    }
}

bool NER::CRFConverter::CanTemplateBeApplied(NER::Sentence^ sentence, int wordIndex, IEnumerable<NER::CRFAttribute^>^ attributeTemplate)
{
	System::Collections::Generic::List<NER::Word^>^ words = sentence->GetWords();
    for each (NER::CRFAttribute^ crfAttribute in attributeTemplate)
    {
        int index = wordIndex + crfAttribute->GetPosition();
        if ((index < 0) || (index >= words->Count))
            return false;
    }
    return true;
}

void NER::CRFConverter::ApplyTemplate(NER::Sentence^ sentence, int wordIndex, IEnumerable<NER::CRFAttribute^>^ attributeTemplate)
{
    for each (CRFAttribute^ crfAttribute in attributeTemplate)
        _stringBuilder->Append(crfAttribute->GetAttributeName() + "[" + crfAttribute->GetPosition() + "]|");
    // Удалить последний |
    _stringBuilder->Remove(_stringBuilder->Length - 1, 1);
    _stringBuilder->Append("=");

	System::Collections::Generic::List<NER::Word^>^ words = sentence->GetWords();
    for each (CRFAttribute^ crfAttribute in attributeTemplate)
    {
        int index = wordIndex + crfAttribute->GetPosition();
        int columnIndex = _attributeIndexTable[crfAttribute->GetAttributeName()];

        System::String^ word = words[index]->GetGraphematicCharacteristics()[columnIndex];
        _stringBuilder->Append(word + "|");
    }
    // Удалить последний |
    _stringBuilder->Remove(_stringBuilder->Length - 1, 1);
    _stringBuilder->Append("\t");
}