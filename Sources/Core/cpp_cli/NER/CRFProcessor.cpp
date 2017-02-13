#include "CRFProcessor.h"
#include <Windows.h>
#include "..\..\..\[libraries]\CRFLib\include\crfsuite.hpp"

using namespace System;

std::string NER::CRFProcessor::TOstring (System::String^ s)
{
	using namespace System::Runtime::InteropServices;
	const char* chars = (const char*)(Marshal::StringToHGlobalAnsi(s)).ToPointer();
	std::string str = chars;
	Marshal::FreeHGlobal(System::IntPtr((void*)chars));
	return str;
}

std::wstring NER::CRFProcessor::TOwstring (System::String^ s)
{
   using namespace System::Runtime::InteropServices;
   const wchar_t* chars = (const wchar_t*)(Marshal::StringToHGlobalUni(s)).ToPointer();
   std::wstring str = chars;
   Marshal::FreeHGlobal(System::IntPtr((void*)chars));
   return str;
}

NER::CRFProcessor::CRFProcessor(System::String^ pathToModel)
{
	 std::string str = TOstring(pathToModel->Replace(L"\\\\", L"\\"));
	 m_tagger = new CRFSuite::Tagger();
	 if (!m_tagger->open(str))
	 {
		 throw (gcnew System::IO::FileNotFoundException("Не удалось открыть CRF-модель."));
	 }
}

CRFSuite::StringList NER::CRFProcessor::Run(System::String^ inputCrfString)
{
	//std::string version = CRFSuite::version();
	//CRFSuite::StringList labels = m_tagger->labels();

	System::String^ separator = System::Environment::NewLine + System::Environment::NewLine;
	array<System::String^>^ sentences = SplitString(inputCrfString, separator); 
	CRFSuite::StringList wholeResult;
	for (int sentenceIndex = 0; sentenceIndex < sentences->Length; ++sentenceIndex)
	{
		CRFSuite::ItemSequence xseq;
		array<System::String^>^ strings = SplitString(sentences[sentenceIndex], "\n");
		
		for (int i = 0; i< strings->Length; ++i)
		{
			array<System::String^>^ attributes = SplitString(strings[i], "\t");
			CRFSuite::Item item;
			for (int j = 0; j < attributes->Length; ++j)
			{
				std::string attribute = GetAttributeInUTF8(attributes[j]);
				item.push_back(CRFSuite::Attribute(attribute));
			}
			xseq.push_back(item);
		}

		CRFSuite::StringList result = m_tagger->tag(xseq);
		wholeResult.insert(wholeResult.end(), result.begin(), result.end());
		wholeResult.push_back("");
	}

	return wholeResult;
}

std::string NER::CRFProcessor::GetAttributeInUTF8(System::String^ s)
{
	std::wstring attributeInUTF16 = TOwstring(s);
	int attributeInUTF8Length = attributeInUTF16.size() * 2 + 1;
	int attributeInUTF8BytesCount = sizeof(char) * attributeInUTF8Length;
	char* pAttributeInUTF8 = new char[attributeInUTF8Length];
	memset(pAttributeInUTF8, 0, attributeInUTF8BytesCount);
	WideCharToMultiByte(CP_UTF8, 0, attributeInUTF16.c_str(), -1, pAttributeInUTF8, attributeInUTF8BytesCount, NULL, NULL);
	std::string result = pAttributeInUTF8;
	delete[] pAttributeInUTF8;
	return result;
}

array<String^>^ NER::CRFProcessor::SplitString(String^ s, String^ separator)
{
	return (s->Split(gcnew array<String^>{ separator }, StringSplitOptions::RemoveEmptyEntries));
}