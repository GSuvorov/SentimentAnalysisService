#pragma once

#include <string>

class CMacroDefinitionGenerator
{
public:
	CMacroDefinitionGenerator(void);
	~CMacroDefinitionGenerator(void);
	///создаем файл с макросами для описания классов-наборов значений характеристик
	const char* MakeTemplates();
private:
	///здесь храним строку с макро-описаниями
	std::string m_sMacroDefinition;

	///создаем один макрос на переданное кол-во значений
	std::string MakeMacroDefinition(unsigned int nValueCount);
	///создаем строку-заголовок макроса на переданное кол-во значений
	std::string MakeMacroDefinitionHeader(unsigned int nValueCount);
	///создаем строку-заголовок класса макроса на переданное кол-во значений
	std::string MakeMacroDefinitionClassCTOR(unsigned int nValueCount);

	///создаем строку функций класса макроса на переданное кол-во значений
	std::string MakeMacroDefinitionClassFunctions(unsigned int nValueCount);

	///преобразует порядковое значение в мап-значение (типа 0х00000001)
	std::string IntToMapHex(unsigned int i);
};
