#include "StdAfx.h"
#include ".\macrodefinitiongenerator.h"
#using <mscorlib.dll>

#define ADD_LINE(x) 	s+=x; s+="\\"; s+="\n";

CMacroDefinitionGenerator::CMacroDefinitionGenerator(void)
{
}

CMacroDefinitionGenerator::~CMacroDefinitionGenerator(void)
{
}

const char* CMacroDefinitionGenerator::MakeTemplates()
{
	//Здесь мы генерируем шаблоны для описания наборов значений мап-характеристик
	//- для каждого набора, отличающегося кол-вом значений генерируется по одному шаблону
	//- на основе одного такого шаблона с именем MAP_FEATURE# (# - кол-во значений) можно создавать классы - наборы значений
	//вот пример шаблона и реального класса, который получится при применении этого шаблона на 2 значения

//ШАБЛОН НА 2 ЗНАЧЕНИЯ:

//#define MAP_FEATURE2(\
//	FeatureName,\
//	Description,\
//	BitSize,\
//	name_1,\
//	desc_1,\
//	name_2,\
//	desc_2) \
//	typedef CMapFeature<BitSize, 2, FeatureName##Type> FeatureName;\
//	class FeatureName##Type\
//	{\
//	public:\
//		FeatureName name_1;\
//		FeatureName name_2;\
//		FeatureName##Type(void)\
//			: name_1(0x00000001, desc_1)\
//			, name_2(0x00000002, desc_2)\
//		{\
//		}\
//		~FeatureName##Type(void){};\
//	};


//ОБЪЯВЛЕНИЕ КЛАССА С ПОМОЩЬЮ ШАБЛОНА:

//MAP_FEATURE2(VerbConstruction, "Опаюю", 2, 
//			vcOnlyVerb, "непереходный глагол без косвенного дополнения", 
//			vcOneObjectPlus, "переходный глагол или непереходный с косвенным дополнением")


//РЕАЛЬНЫЙ КОД, ПОЛУЧАЮЩИЙСЯ ПРИ РЕАЛИЗАЦИИ ШАБЛОНА:

//typedef CAMTMap<2, 2> VerbConstruction_value;
//
//class VerbConstruction
//{
//public:
//	VerbConstruction_value vcOnlyVerb;
//	VerbConstruction_value vcOneObjectPlus;
//
//	CTestFeature(void)
//		: vcOnlyVerb(0x00000001, "непереходный глагол без косвенного дополнения")
//		, vcOneObjectPlus(0x00000002, "переходный глагол или непереходный с косвенным дополнением")
//	{
//	}
//	~CTestFeature(void);
//};


	//ТАК ВОТ ЗДЕСЬ МЫ ПОСЛЕДОВАТЕЛЬНО СОБИРАЕМ ШАБЛОНЫ НА КОЛ-ВА ЗНАЧЕНИЙ ОТ 1 до 32 (макс возможное)

	m_sMacroDefinition = "";

	m_sMacroDefinition+= "#pragma once";
	m_sMacroDefinition+= "\n";
	unsigned int i = 1;
	while(i <= 32){
		//вызываем функцию, которая генерирует строку с шаблоном на i значений
		m_sMacroDefinition+= MakeMacroDefinition(i);
		m_sMacroDefinition+= MakeMacroDefinitionClassCTOR(i);
		m_sMacroDefinition+= MakeMacroDefinitionClassFunctions(i);
		m_sMacroDefinition+= "	};\\";
		m_sMacroDefinition+= "\n";
		m_sMacroDefinition+= "\n";

		i++;
	}

	return m_sMacroDefinition.c_str();
}

//создаем один макрос на переданное кол-во значений
std::string CMacroDefinitionGenerator::MakeMacroDefinition(unsigned int nValueCount)
{
	std::string s;
	s = MakeMacroDefinitionHeader(nValueCount);

	return s;
}


///создаем строку-заголовок макроса на переданное кол-во значений
std::string CMacroDefinitionGenerator::MakeMacroDefinitionHeader(unsigned int nValueCount)
{
	std::string s;
	char bff[20];


//#define MAP_FEATURE2(\
//	FeatureName,\
//	BitSize,\
//	name_1,\
//	desc_1,\
//	name_2,\
//	desc_2)\
//	class FeatureName##Type;\
//	typedef CAMTMap<BitSize, 2> FeatureName##_value;\
//	class FeatureName\
//	{\
//	public:\

	s = "#define MAP_FEATURE";
	s+=ltoa(nValueCount, bff, 10);
	ADD_LINE("(");
	ADD_LINE("	FeatureName,");
	ADD_LINE("	Description,");
	ADD_LINE("	BitSize,");

	//теперь добавляем количественно зависимые параметры
	unsigned int i = 1;
	while(i <= nValueCount){

		s+="	name_";
		s+=ltoa(i, bff, 10);
		s+=",\\";
		s+="\n";
		s+="	desc_";
		s+=ltoa(i, bff, 10);

		if(i == nValueCount){
			s+=")";
		}
		else{
			s+=",";
		}
		s+="\\";
		s+="\n";

		i++;
	}
	ADD_LINE("	class FeatureName##Type;");
	s+="	typedef CMapFeature<BitSize, ";
	s+=ltoa(nValueCount, bff, 10);
	s+=", FeatureName##Type> FeatureName;\\";
	s+="\n";
	ADD_LINE("	class FeatureName##Type");
	ADD_LINE("		: public CBaseType");
	ADD_LINE("	{");
	ADD_LINE("	public:");

	return s;
}

///создаем строку-заголовок класса макроса на переданное кол-во значений
std::string CMacroDefinitionGenerator::MakeMacroDefinitionClassCTOR(unsigned int nValueCount)
{
	std::string s;
	char bff[20];

//		FeatureName name_1;\
//		FeatureName name_2;\
//		FeatureName(void)\
//			: name_1(0x00000001, desc_1)\
//			, name_2(0x00000002, desc_2)\
//		{\
//		}\
//		~FeatureName(void){};\

	//теперь добавляем количественно зависимые параметры
	unsigned int i = 1;
	while(i <= nValueCount){

		s+="		FeatureName name_";
		s+=ltoa(i, bff, 10);
		s+=";\\";
		s+="\n";

		i++;
	}
	s+="		FeatureName##Type(void)\\";
	s+="\n";
	//теперь добавляем количественно зависимые параметры
	i = 1;
	while(i <= nValueCount){
		if(i == 1){
			s+="			: ";
		}
		else{
			s+="			, ";
		}
		s+="name_";
		s+=ltoa(i, bff, 10);
		s+="(";
		s+=ltoa(i - 1, bff, 10);//IntToMapHex(i);
		//s+=", desc_";
		//s+=ltoa(i, bff, 10);
		s+=")\\";
		s+="\n";

		i++;
	}
	ADD_LINE("		{");
	ADD_LINE("		}");
	ADD_LINE("		~FeatureName##Type(void){};");

	return s;
}

///создаем строку функций класса макроса на переданное кол-во значений
std::string CMacroDefinitionGenerator::MakeMacroDefinitionClassFunctions(unsigned int nValueCount)
{
/*
			unsigned int GetValueCount(){return `nValueCount`;};
			const char* GetTypeName(){return #FeatureName;};
			const char* GetTypeDescription(){return Description;};
			const char* GetValueName(unsigned int n);
			const char* GetValueDescription(unsigned int n);
*/
	std::string s;
	char bff[20];
	ADD_LINE("			unsigned int GetValueCount(){");
	s+= "				return ";
	s+= ltoa(nValueCount, bff, 10);
	ADD_LINE(";};");
	ADD_LINE("			const char* GetTypeName(){return #FeatureName;};");
	ADD_LINE("			const char* GetTypeDescription(){return Description;};");
	ADD_LINE("			const char* GetValueName(unsigned int n){");
	ADD_LINE("				switch(n){");
	unsigned int i = 1;
	while(i <= nValueCount){
		s+= "				case ";
		s+=ltoa(i - 1, bff, 10);
		s+= ": return #name_";
		s+=ltoa(i, bff, 10);
		s+=";\\";
		s+="\n";
		i++;
	};
	ADD_LINE("				default: SS_THROW(L\"некорректный номер значения\");");
	ADD_LINE("				}");
	ADD_LINE("				return \"некорректный номер значения\"; ");
	ADD_LINE("			};");

	ADD_LINE("			const char* GetValueDescription(unsigned int n){");
	ADD_LINE("				switch(n){");
	i = 1;
	while(i <= nValueCount){
		s+= "				case ";
		s+=ltoa(i - 1, bff, 10);
		s+= ": return desc_";
		s+=ltoa(i, bff, 10);
		s+=";\\";
		s+="\n";
		i++;
	};
	ADD_LINE("				default: SS_THROW(L\"некорректный номер значения\");");
	ADD_LINE("				}");
	ADD_LINE("				return \"некорректный номер значения\"; ");
	ADD_LINE("			};");

	return s;
}
std::string CMacroDefinitionGenerator::IntToMapHex(unsigned int i)
{
	unsigned char uc = i - 1;
	switch(uc){
		case 0: return "0x00000001";
			break;
		case 1: return "0x00000002";
			break;
		case 2: return "0x00000004";
			break;
		case 3: return "0x00000008";
			break;
		case 4: return "0x00000010";
			break;
		case 5: return "0x00000020";
			break;
		case 6: return "0x00000040";
			break;
		case 7: return "0x00000080";
			break;
		case 8: return "0x00000100";
			break;
		case 9: return "0x00000200";
			break;
		case 10: return "0x00000400";
			break;
		case 11: return "0x00000800";
			break;
		case 12: return "0x00001000";
			break;
		case 13: return "0x00002000";
			break;
		case 14: return "0x00004000";
			break;
		case 15: return "0x00008000";
			break;
		case 16: return "0x00010000";
			break;
		case 17: return "0x00020000";
			break;
		case 18: return "0x00040000";
			break;
		case 19: return "0x00080000";
			break;
		case 20: return "0x00100000";
			break;
		case 21: return "0x00200000";
			break;
		case 22: return "0x00400000";
			break;
		case 23: return "0x00800000";
			break;
		case 24: return "0x01000000";
			break;
		case 25: return "0x02000000";
			break;
		case 26: return "0x04000000";
			break;
		case 27: return "0x08000000";
			break;
		case 28: return "0x10000000";
			break;
		case 29: return "0x20000000";
			break;
		case 30: return "0x40000000";
			break;
		case 31: return "0x80000000";
			break;
		default:
			throw 0;
	};
}