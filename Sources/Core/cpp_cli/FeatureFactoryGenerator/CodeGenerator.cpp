#include "StdAfx.h"
#include ".\codegenerator.h"
#using <mscorlib.dll>

namespace FeatureFactoryGenerator
{
	CCodeGenerator::CCodeGenerator(void)
	{
	}

	CCodeGenerator::~CCodeGenerator(void)
	{
	}
#define S_ADD(x) s = s->Concat(s, new System::String(x))
#define S_ADD_S(x) s = s->Concat(s, x)
#define S_LINE(x) s = s->Concat(s, new System::String(x)); s = s->Concat(s, new System::String("\n"))
	///сгенерирует код в стрингу по переданному хранилищу характеристик
	System::String* CCodeGenerator::Generate(CFeatureStorage* pFeatureStorage)
	{

		System::String* s = "";
		S_LINE("#pragma once");
		S_LINE("");
		S_LINE("#include \"MapFeature.h\"");
		S_LINE("#include \"PercentFeature.h\"");
		S_LINE("#include \"MapMacro.h\"");
		S_LINE("#include \"IntegerMacro.h\"");
		S_LINE("#include \"BitSize.h\"");
		S_LINE("namespace SS");
		S_LINE("{");
		S_LINE("	namespace Core");
		S_LINE("	{");
		S_LINE("		namespace Features");
		S_LINE("		{");
		S_LINE("			namespace Types");
		S_LINE("			{");
		S_LINE("");

		CFeatureGroup* pFG = pFeatureStorage->FirstGroup();
		while(pFG){
			s = s->Concat(s, Generate(pFG));
			pFG = pFeatureStorage->NextGroup();
		}

		S_LINE("			}");
		S_LINE("		}");
		S_LINE("	}");
		S_LINE("}");
		return s;
	}
	///сгенерирует код в стрингу по переданной группе характеристик
	System::String* CCodeGenerator::Generate(CFeatureGroup* pFeatureGroup)
	{
		System::String* s = "";
		CFeature* pF = pFeatureGroup->First();
		while(pF){
			s = s->Concat(s, Generate(pF));
			pF = pFeatureGroup->Next();
		}
		return s;
	}

	///сгенерирует код в стрингу по переданной характеристике
	System::String* CCodeGenerator::Generate(CFeature* pFeature)
	{
		System::String* s = "";

		if(pFeature->GetFeatureType()->Equals("Map")){
			s = GenerateMap(pFeature);
		}
		else if(pFeature->GetFeatureType()->Equals("Integer")){
			s = GenerateInteger(pFeature);
		}
		else if(pFeature->GetFeatureType()->Equals("Percent")){
			s = GeneratePercent(pFeature);
		}

		return s;
	}

	///сгенерирует код в стрингу по переданной характеристике
	System::String* CCodeGenerator::GenerateMap(CFeature* pFeature)
	{
	//class FeatureName##Type;\
	//typedef CMapFeature<BitSize, 2, FeatureName##Type> FeatureName;\
	//class FeatureName##Type\
	//	: public CBaseType\
	//{\
	//public:\
	//	FeatureName name_1;\
	//	FeatureName name_2;\
	//	FeatureName##Type(void)\
	//		: name_1(0)\
	//		, name_2(1)\
	//	{\
	//	}\
	//	~FeatureName##Type(void){};\
	//		unsigned int GetValueCount(){\
	//			return 2;};\
	//		const char* GetTypeName(){return #FeatureName;};\
	//		const char* GetTypeDescription(){return Description;};\
	//		const char* GetValueName(unsigned int n){\
	//			switch(n){\
	//			case 0: return #name_1;\
	//			case 1: return #name_2;\
	//			default: SS_THROW("некорректный номер значения");\
	//			}\
	//		};\
	//		const char* GetValueDescription(unsigned int n){\
	//			switch(n){\
	//			case 0: return desc_1;\
	//			case 1: return desc_2;\
	//			default: SS_THROW("некорректный номер значения");\
	//			}\
	//		};\
	//};
		System::String* s = "";
		S_ADD("class ");
		S_ADD_S(pFeature->GetFeatureName());
		S_LINE("Type;");

		S_ADD("typedef CMapFeature<8, ");
		System::UInt32 n;
		n = pFeature->GetValueCount();

		System::UInt32 nValueCount = 0;
		//подсчитаем кол-во значений
		CFeatureValue* pFV;
		pFV = pFeature->FirstValue();
		while(pFV){
			if(pFV->IsEnabled()){
				nValueCount++;
			}
			pFV = pFeature->NextValue();
		}

		if(nValueCount == 0) throw new System::Exception("нет значений мап-хар-ки");

		S_ADD_S(nValueCount.ToString());
		S_ADD(", ");
		S_ADD_S(pFeature->GetFeatureName());
		S_ADD("Type> ");
		S_ADD_S(pFeature->GetFeatureName());
		S_LINE(";");

		S_ADD("class ");
		S_ADD_S(pFeature->GetFeatureName());
		S_LINE("Type");
		S_LINE("	: public CBaseType");
		S_LINE("{");
		S_LINE("public:");

		pFV = pFeature->FirstValue();
		while(pFV){
			if(pFV->IsEnabled()){
				S_ADD( "	");
				S_ADD_S(pFeature->GetFeatureName());
				S_ADD( " ");
				S_ADD_S(pFV->GetValueName());
				S_LINE(";");
			}
			pFV = pFeature->NextValue();
		}

		S_ADD( "	");
		S_ADD_S(pFeature->GetFeatureName());
		S_LINE("Type(void)");

		pFV = pFeature->FirstValue();
		while(pFV->IsEnabled() == false){
			pFV = pFeature->NextValue();
		}
		S_ADD( "		: ");
		S_ADD_S(pFV->GetValueName());
		S_LINE("(0)");

		pFV = pFeature->NextValue();
		n = 1;
		while(pFV){
			if(pFV->IsEnabled()){
				S_ADD( "		, ");
				S_ADD_S(pFV->GetValueName());
				S_ADD("(");
				S_ADD_S(n.ToString());
				S_LINE(")");
				n++;
			}
			pFV = pFeature->NextValue();
		}
		S_LINE("	{");
		S_LINE("	}");
		S_LINE("");

		S_ADD( "	~");
		S_ADD_S(pFeature->GetFeatureName());
		S_LINE("Type(void){};");
		S_LINE("");

		S_ADD( "	unsigned int GetValueCount(void) const {return ");
		S_ADD_S(nValueCount.ToString());
		S_LINE(";};");
		S_LINE("");

		S_ADD( "	const char* GetTypeName() const {return \"");
		S_ADD_S(pFeature->GetFeatureName());
		S_LINE("\";};");
		S_LINE("");

		S_ADD( "	const char* GetTypeDescription() const {return \"");
		S_ADD_S(pFeature->GetFeatureDescription());
		S_LINE("\";};");
		S_LINE("");

		S_LINE("	const char* GetValueName(unsigned int n) const {");
		S_LINE("		switch(n){");
		pFV = pFeature->FirstValue();
		n = 0;
		while(pFV){
			if(pFV->IsEnabled()){
				S_ADD( "		case ");
				S_ADD_S(n.ToString());
				S_ADD( ": return \"");
				S_ADD_S(pFV->GetValueName());
				S_LINE("\";");
				n++;
			}
			pFV = pFeature->NextValue();
		}
		S_LINE("		default: SS_THROW(L\"некорректный номер значения\");");
		S_LINE("		}");
		S_LINE("		return \"некорректный номер значения\"; ");
		S_LINE("	};");
		S_LINE("");

		S_LINE("	const char* GetValueDescription(unsigned int n) const {");
		S_LINE("		switch(n){");
		pFV = pFeature->FirstValue();
		n = 0;
		while(pFV){
			if(pFV->IsEnabled()){
				S_ADD( "		case ");
				S_ADD_S(n.ToString());
				S_ADD( ": return \"");
				S_ADD_S(pFV->GetValueDescription());
				S_LINE("\";");
				n++;
			}
			pFV = pFeature->NextValue();
		}
		S_LINE("		default: SS_THROW(L\"некорректный номер значения\");");
		S_LINE("		}");
		S_LINE("		return \"некорректный номер значения\"; ");
		S_LINE("	};");
		S_LINE("};");
		S_LINE("");
		S_LINE("");

		return s;
	}
	///сгенерирует код в стрингу по переданной характеристике
	System::String* CCodeGenerator::GenerateInteger(CFeature* pFeature)
	{
	//class FeatureName##Type;\
	//typedef BaseClass<FeatureName##Type> FeatureName;\
	//class FeatureName##Type\
	//	: public CBaseType\
	//{\
	//public:\
	//		const char* GetTypeName(){return #BaseClass;};\
	//		const char* GetTypeDescription(){return GroupDescription;};\
	//		const char* GetValueName(unsigned int n){return #FeatureName;};\
	//		const char* GetValueDescription(unsigned int n){return Description;};\
	//};
		System::String* s = "";

		S_ADD("class ");
		S_ADD_S(pFeature->GetFeatureName());
		S_LINE("Type;");

		S_ADD("typedef CIntegerFeature<");
		S_ADD_S(pFeature->GetFeatureName());
		S_ADD("Type> ");
		S_ADD_S(pFeature->GetFeatureName());
		S_LINE(";");

		S_ADD("class ");
		S_ADD_S(pFeature->GetFeatureName());
		S_LINE("Type");
		S_LINE("	: public CBaseType");
		S_LINE("{");
		S_LINE("public:");

		S_ADD( "	const char* GetTypeName() const {return \"");
		S_ADD("CIntegerFeature");
		S_LINE("\";};");
		S_LINE("");

		S_ADD( "	const char* GetTypeDescription() const {return \"");
		S_ADD_S(pFeature->GetFeatureTypeDescription());
		S_LINE("\";};");
		S_LINE("");

		S_ADD( "	const char* GetValueName(unsigned int n) const {return \"");
		S_ADD_S(pFeature->GetFeatureName());
		S_LINE("\";};");
		S_LINE("");

		S_ADD( "	const char* GetValueDescription(unsigned int n) const {return \"");
		S_ADD_S(pFeature->GetFeatureDescription());
		S_LINE("\";};");
		S_LINE("");

		S_LINE("};");
		S_LINE("");
		S_LINE("");

		return s;
	}
	///сгенерирует код в стрингу по переданной характеристике
	System::String* CCodeGenerator::GeneratePercent(CFeature* pFeature)
	{
		System::String* s = "";
		S_ADD("class ");
		S_ADD_S(pFeature->GetFeatureName());
		S_LINE("Type;");

		S_ADD("typedef CPercentFeature<");
		S_ADD_S(pFeature->GetFeatureName());
		S_ADD("Type> ");
		S_ADD_S(pFeature->GetFeatureName());
		S_LINE(";");

		S_ADD("class ");
		S_ADD_S(pFeature->GetFeatureName());
		S_LINE("Type");
		S_LINE("	: public CBaseType");
		S_LINE("{");
		S_LINE("public:");

		S_ADD( "	const char* GetTypeName() const {return \"");
		S_ADD("CPercentFeature");
		S_LINE("\";};");
		S_LINE("");

		S_ADD( "	const char* GetTypeDescription() const {return \"");
		S_ADD_S(pFeature->GetFeatureTypeDescription());
		S_LINE("\";};");
		S_LINE("");

		S_ADD( "	const char* GetValueName(unsigned int n) const {return \"");
		S_ADD_S(pFeature->GetFeatureName());
		S_LINE("\";};");
		S_LINE("");

		S_ADD( "	const char* GetValueDescription(unsigned int n) const {return \"");
		S_ADD_S(pFeature->GetFeatureDescription());
		S_LINE("\";};");
		S_LINE("");

		S_LINE("};");
		S_LINE("");
		S_LINE("");
		return s;
	}
}