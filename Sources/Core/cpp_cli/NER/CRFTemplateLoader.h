#pragma once
#include "CRFTemplate.h"
#include "CRFNgram.h"

namespace NER
{
	using namespace System;
	using namespace System::Collections::Generic;
	using namespace System::IO;
	using namespace System::Text::RegularExpressions;

    /// Загрузчик шаблонов
    ref class CRFTemplateLoader
    {
	private:
        // Названия групп внутри регулярных выражений
        static System::String^ Templates = "Templates";
        static System::String^ Template = "Template";
        static System::String^ Fields = "Fields";

        /// Регулярное выражение для выделения шаблонов
        static Regex^ _templates = gcnew Regex("templates\\s*=\\s*\\((?<" + Templates + ">(\\s*.+)*)\\s*\\)", RegexOptions::IgnoreCase);
        
        /// регулярное выражение для выделения одного шаблона
        static Regex^ _template = gcnew Regex("\\((?<" + Template + ">[^(]*)\\),", RegexOptions::IgnoreCase);
        
        /// Регулярное выражение для выделения названий столбцов
        static Regex^ _fields = gcnew Regex("fields\\s*=\\s*'(?<" + Fields + ">[^']*)'", RegexOptions::IgnoreCase);

	public:
        ///Загрузить файл шаблона
		///@param filePath Путь к файлу шаблона
		///@return файл шаблона
        NER::CRFTemplate^ Load(System::String^ filePath);

	private:
        /// Извлечь шаблоны аттрибутов
        /// @param fileString - Содержимое файла-шаблона
        /// @return - Шаблоны аттрибутов
        static List<NER::Ngram^>^ ExtractAttributeTemplates(System::String^ fileString);

        /// Извлечь название аттрибута
        /// @param attrStr - Строка, содержащая название аттрибута
        /// @return Название аттрибута
        static System::String^ ParseAttributeName(System::String^ attrStr);


        /// Извлечь строки, соответствующие шаблонам аттрибутов
        /// @param fileString - Содержимое файла-шаблона
        /// @return Строки, соответствующие шаблонам аттрибутов
        static array<System::String^>^ ExtractAttributeTemplateStrings(System::String^ fileString);

        /// Извлечь названия столбцов
        /// @param fileString - Содержимое файла-шаблона
        /// @return Названия столбцов
        static array<System::String^>^ ExtractColumnNames(System::String^ fileString);
    };
}
