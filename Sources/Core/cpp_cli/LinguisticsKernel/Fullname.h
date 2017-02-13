#pragma once

#include "..\..\..\..\Sources\Core\ASSInterface\IDictionaryManager.h"
#include "..\..\..\..\Sources\Core\ASSInterface\IAMConverter.h"
#include "..\..\..\..\Sources\Core\ASSInterface\IDictionaryManager.h"

using namespace System;
using namespace System::Text;
using namespace System::Xml::Linq;
using namespace System::Xml;
using namespace System::Linq;
using namespace System::Runtime::InteropServices;

using namespace SS::Core::Features;

public __gc class CFullname
{
    public:
        CFullname(Lingvistic::ICoreferenceResolution* pCoreference, 
            Collections::Generic::Dictionary<String*, XmlNode*>* dictionary);
        virtual ~CFullname();

        String* ToString(XElement* node);
        String* ToString(XElement* node, String* caseType);

        static void SetLoadManager(SS::Interface::Core::ResourceManagers::ILoadManager* pLoadManager);

    private:
        String* ToString(XElement* node, Types::CaseType caseType, Types::GenderType gender);

        bool TryGetFullname(XElement* node, StringBuilder* text);

        String* GetWordForm(String* word, Types::CaseType caseType, Types::GenderType gender);

        String* PersonToString(XElement* node, Types::CaseType caseType, Types::GenderType gender);
        String* DefaultToString(XElement* node, Types::CaseType caseType, Types::GenderType gender);
        String* PrefixAndProffesion(XElement* node, Types::CaseType caseType, Types::GenderType gender);

        Types::CaseType GetCaseType(String* caseType);
        Types::GenderType GetGenderType(String* genderType);
        void GetWordCaseType(XElement* node, Types::CaseType &caseType);
        void GetWordGenderType(XElement* node, Types::GenderType &genderType);

        void AppendWord(StringBuilder* text, String* word);

        SemanticResolution::CoreferenceResolution* m_pCoreference;
        Collections::Generic::Dictionary<String*, XmlNode*>* m_Dictionary;

        static SS::Interface::Core::ResourceManagers::ILoadManager* m_pLoadManager;
        static SS::Interface::Core::Dictionary::IMorphologyAnalyser* m_pMorphoAnalyzer;
        static SS::Interface::Core::AMConverter::IAMConverterMorpho* m_pAMConverter;
};

