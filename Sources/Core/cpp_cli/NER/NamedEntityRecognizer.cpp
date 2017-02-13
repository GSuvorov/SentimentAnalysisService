#include "NamedEntityRecognizer.h"
#include "CRFTemplateLoader.h"

using System::Collections::Generic::List;

NER::NamedEntityRecognizer::NamedEntityRecognizer(System::String^ modelPath, System::String^ templatePath)
{
	m_processor = gcnew NER::CRFProcessor(modelPath);
	m_template = GetTemplate(templatePath);
	m_converter = gcnew CRFConverter(m_template);
}

NER::CRFTemplate^ NER::NamedEntityRecognizer::GetTemplate(System::String^ templatePath)
{
	try
	{
		NER::CRFTemplateLoader^ templateLoader = gcnew CRFTemplateLoader();
		NER::CRFTemplate^ result = templateLoader->Load(templatePath);
		CheckTemplateOnly(result);
		return result;
	}
	catch (System::Exception^ ex)
	{
		throw gcnew System::Exception("Неверный формат CRF-шаблона " + templatePath + ": " + ex);
	}
	return nullptr;
}

// Запустить обработчик
// @sourceString - Текст разбитый на слова
List<System::String^>^ NER::NamedEntityRecognizer::Run(System::String^ sourceString)
{
	List<NER::Sentence^>^ sentences = GetSentencesWithAdditionalInfo(sourceString);
	CheckTemplateAndResult(m_template, sentences);
	System::String^ inputCrfString = m_converter->ToCRFFormat(sentences);
	//WriteCrfResultToFile(m_tempResult, inputCrfString);
	std::vector<std::string> result = m_processor->Run(inputCrfString);

	List<System::String^>^ stringList = gcnew List<System::String^>();
	for(unsigned int i = 0; i < result.size() ; ++i)
		stringList->Add(gcnew String(result[i].c_str()));

	return stringList;
}

// Вставить дополнительные характеристики для слов
// @sourceString - Исходный формат слов
// @returns - Слова с характеристиками
List<NER::Sentence^>^ NER::NamedEntityRecognizer::GetSentencesWithAdditionalInfo(System::String^ sourceString)
{
	List<NER::Sentence^>^ result = gcnew List<Sentence^>();
	array<System::String^>^ words = sourceString->Split(gcnew array<System::String^> {System::Environment::NewLine}, System::StringSplitOptions::None);
    
	NER::Sentence^ currentSentence = gcnew Sentence();
	for each (System::String^ word in words)
	{
		if (System::String::IsNullOrEmpty(word))
		{
			result->Add(currentSentence);
			currentSentence = gcnew NER::Sentence();
		}
		else
			currentSentence->AddWord(gcnew Word(word));
	}
	if (currentSentence->GetWords()->Count > 0)
		result->Add(currentSentence);

	return result;
}

// Запись промежуточного результата в файл
// @destenationFilePath - 
// @stringToWrite - 
// Временный функционал
void NER::NamedEntityRecognizer::WriteCrfResultToFile(System::String^ destenationFilePath, System::String^ stringToWrite)
{
	using System::IO::StreamWriter;
	StreamWriter^ sw;
	try
	{
		sw = gcnew StreamWriter(destenationFilePath, true, System::Text::Encoding::UTF8);
		sw->Write(stringToWrite);
	}
	catch (Exception^ ex)
	{
		throw ex;
	}
	finally
	{
		sw->Close();
	}
}

void NER::NamedEntityRecognizer::CheckTemplateAndResult(NER::CRFTemplate^ crfTemplate, List<NER::Sentence^>^ sentences)
{
	if (sentences->Count > 0)
	{
		int columnsCount = sentences[0]->GetWords()[0]->GetGraphematicCharacteristics()->Count;
		if (columnsCount != crfTemplate->GetColumnNames()->Length)
		{
			throw gcnew System::Exception("Не совпадает количество столбцов указанное в шаблоне (" + crfTemplate->GetColumnNames()->Length +
								") с количеством столбцов в обрабатываемом файле (" + columnsCount + ")");
		}
	}
}

// Проверить правильность шаблона
// @template - Шаблон
void NER::NamedEntityRecognizer::CheckTemplateOnly(NER::CRFTemplate^ crfTemplate)
{
	for each (Ngram^ ngram in crfTemplate->GetNgramTemplates())
	{
		for each (CRFAttribute^ crfAttribute in ngram->GetParts())
		{
			List<System::String^>^ columnNamesList = gcnew List<System::String^>(crfTemplate->GetColumnNames());
			if (columnNamesList->Contains(crfAttribute->GetAttributeName()))
				continue;

			System::Text::StringBuilder^ stringBuilder = gcnew System::Text::StringBuilder();
			for each (System::String^ columnName in crfTemplate->GetColumnNames())
				stringBuilder->Append(System::Environment::NewLine + columnName);
			throw gcnew System::Exception("Аттрибут " + crfAttribute->GetAttributeName() +
								" не содержащится в названиях столбцов CRF-файла-шаблона: " +
								stringBuilder);
		}
	}
}