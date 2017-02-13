#include "CRFTemplateLoader.h"
using System::Collections::Generic::List;

// Загрузить файл шаблона
NER::CRFTemplate^ NER::CRFTemplateLoader::Load(System::String^ filePath)
{
    StreamReader^ sr = gcnew StreamReader(filePath);
    System::String^ fileString = sr->ReadToEnd();

    array<System::String^>^ columnNames = ExtractColumnNames(fileString);
    List<Ngram^>^ attributeTemplates = ExtractAttributeTemplates(fileString);

    return gcnew CRFTemplate(columnNames, attributeTemplates);
}

// Извлечь шаблоны аттрибутов
// @param fileString - Содержимое файла-шаблона
// @return - Шаблоны аттрибутов
List<NER::Ngram^>^ NER::CRFTemplateLoader::ExtractAttributeTemplates(System::String^ fileString)
{
    array<System::String^>^ attributeTemplateStrings = ExtractAttributeTemplateStrings(fileString);

    List<NER::Ngram^>^ attributeTemplates = gcnew List<NER::Ngram^>(attributeTemplateStrings->Length);
    for each (System::String^ str in attributeTemplateStrings)
    {
        MatchCollection^ matchCollection = _template->Matches(str);
        if(matchCollection->Count == 0) 
            continue;

        List<CRFAttribute^>^ attributeTemplate = gcnew List<CRFAttribute^>(matchCollection->Count);
        for each (Match^ currentMatch in matchCollection)
        {
            System::String^ oneTemplate = currentMatch->Groups[Template]->Value;
            array<System::String^>^ pair = oneTemplate->Split(gcnew array<System::Char> {','});

            System::String^ attributeName = ParseAttributeName(pair[0]);
            int position = System::Int32::Parse(pair[1]);

            attributeTemplate->Add(gcnew CRFAttribute(attributeName, position));
        }
        attributeTemplates->Add(gcnew Ngram(attributeTemplate));
    }
    return attributeTemplates;
}

// Извлечь название аттрибута
// @param attrStr - Строка, содержащая название аттрибута
// @return - Название аттрибута
System::String^ NER::CRFTemplateLoader::ParseAttributeName(System::String^ attrStr)
{
    int startIndex = attrStr->IndexOf('\'') + 1;
    int endIndex = attrStr->IndexOf('\'', startIndex);
    return attrStr->Substring(startIndex, endIndex - startIndex);
}

// Извлечь названия столбцов
// @param fileString - Содержимое файла-шаблона
// @return - Названия столбцов
array<System::String^>^ NER::CRFTemplateLoader::ExtractColumnNames(System::String^ fileString)
{
    Match^ match = _fields->Match(fileString);
    return match->Groups[Fields]->Value->Split(' ', '\t', '\n');
}

// Извлечь строки, соответствующие шаблонам аттрибутов
// @param fileString - Содержимое файла-шаблона
// @return - Строки, соответствующие шаблонам аттрибутов
array<System::String^>^ NER::CRFTemplateLoader::ExtractAttributeTemplateStrings(System::String^ fileString)
{
    Match^ templatesMatch = _templates->Match(fileString);
    System::String^ templates = templatesMatch->Groups[Templates]->Value;

    templates = Regex::Replace(templates, "\\s*\\(\\s*\\(\\s*", "(");
    templates = Regex::Replace(templates, ",\\s*\\)\\s*,", ",\n");

    return templates->Split(gcnew array<System::Char> { '\n' }, StringSplitOptions::RemoveEmptyEntries);
}