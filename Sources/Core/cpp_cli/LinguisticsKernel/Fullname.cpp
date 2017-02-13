#include "StdAfx.h"
#include "Fullname.h"
#include <list>

using namespace Lingvistic;

CFullname::CFullname(
    ICoreferenceResolution* pCoreference, 
    Collections::Generic::Dictionary<String*, XmlNode*>* dictionary)
{
    m_pCoreference = pCoreference;
    m_Dictionary = dictionary;
}

CFullname::~CFullname()
{
}

void CFullname::SetLoadManager(SS::Interface::Core::ResourceManagers::ILoadManager* pLoadManager)
{
    using namespace SS::Interface::Core::Dictionary;
    m_pLoadManager = pLoadManager;

    if(!m_pAMConverter)
    {
        IDictionary* pDict = (IDictionary*) 
		    pLoadManager->GetInterface(L"./core/linguistics/Dictionary.dll", CLSID_IDictionary, IID_IDictionary);
    	m_pMorphoAnalyzer = pDict->GetMorphologyAnalyser();
        m_pAMConverter = (SS::Interface::Core::AMConverter::IAMConverterMorpho*)
    	    pLoadManager->GetInterface(L"./core/linguistics/AMConverter.dll", CLSID_AMConverterMorpho, IID_AMConverterMorpho);

    }
}

String* CFullname::ToString(
    XElement* node,
    Types::CaseType caseType,
    Types::GenderType gender)
{
    String* result = "";
    if(caseType.Equal(Values::CaseType.ctNominative))
    {
        result = SemanticResolution::ServiceFunction::GetAttributeValue(node, 
            __box(BlockAttribute::FULLNAME)->ToString());
    }
    if(result == "")
    {
        if(m_pMorphoAnalyzer)
        {
            switch(m_pCoreference->GetCoreferenceType(node))
            {
                case CoreferenceType::Person:
                    result = PersonToString(node, caseType, gender);
                    break;
                default:
                    result = DefaultToString(node, caseType, gender);
                    break;
            }
        }
    }
    return result;
}

String* CFullname::ToString(
    XElement* node,
    String* caseType)
{
    Types::CaseType phraseCase = GetCaseType(caseType);
    Types::GenderType gender = Values::GenderType.Undefined;
    GetWordGenderType(node, gender);
    return ToString(node, phraseCase, gender);
}

String* CFullname::ToString(XElement* node)
{
    Types::CaseType caseType = Values::CaseType.Undefined;
    Types::GenderType gender = Values::GenderType.Undefined;
    GetWordCaseType(node, caseType);
    GetWordGenderType(node, gender);
    return ToString(node, caseType, gender);
}

Types::CaseType CFullname::GetCaseType(String* caseType)
{
    Types::CaseType type = Values::CaseType.Undefined;
    if(caseType->Equals("N"))
        type = Values::CaseType.ctNominative;
    else if(caseType->Equals("G"))
        type = Values::CaseType.ctGenitive;
    else if(caseType->Equals("D"))
        type = Values::CaseType.ctDative;
    else if(caseType->Equals("A"))
        type = Values::CaseType.ctAccusative;
    else if(caseType->Equals("I"))
        type = Values::CaseType.ctInstrumental;
    else if(caseType->Equals("L"))
        type = Values::CaseType.ctLocative;
    else if(caseType->Equals("C"))
        type = Values::CaseType.ctCommon;
    return type;
}

Types::GenderType CFullname::GetGenderType(String* genderType)
{
    Types::GenderType gender = Values::GenderType.Undefined;
    if(genderType->Equals("F"))
        gender = Values::GenderType.gtFemale;
    else if(genderType->Equals("M"))
        gender = Values::GenderType.gtMasculine;
    else if(genderType->Equals("N"))
        gender = Values::GenderType.gtNeutral;
    else if(genderType->Equals("G"))
        gender = Values::GenderType.gtGeneral;
    return gender;
}

void CFullname::GetWordCaseType(XElement* node, Types::CaseType &caseType)
{
    String* wordInfo = SemanticResolution::ServiceFunction::GetAttributeValue(node, __box(BlockAttribute::MI)->ToString());
    String* type = "";
    if(wordInfo != "")
        type = wordInfo->Substring(0, 1);
    caseType = GetCaseType(type);
}

void CFullname::GetWordGenderType(XElement* node, Types::GenderType &genderType)
{
    String* wordInfo = SemanticResolution::ServiceFunction::GetAttributeValue(node, __box(BlockAttribute::MI)->ToString());
    String* type = "";
    if(wordInfo != "")
        type = wordInfo->Substring(1, 1);
    genderType = GetGenderType(type);
}

String* CFullname::GetWordForm(
    String* word,
    Types::CaseType caseType,
    Types::GenderType gender)
{
    String* result = "";
    if(word != "")
    {
        System::Collections::Generic::List<String*>* words = SemanticResolution::ServiceFunction::SplitString(word, " ");
        for(int i = 0; i < words->Count; ++i)
        {
            String* form = "";
            list<SS::Dictionary::Types::TWordInfo> info;
            const wchar_t* pWord = (const wchar_t*)(Marshal::StringToHGlobalUni(words->Item[i])).ToPointer();
            m_pMorphoAnalyzer->GetWordInfo(pWord, &info, SS::Dictionary::Types::EFindZone::efzOnlySearch);
            std::list<SS::Dictionary::Types::TWordInfo> infoDetect;
            for(std::list<SS::Dictionary::Types::TWordInfo>::iterator it = info.begin();
                (form == "") && (it != info.end());
                ++it)
            {	
                SS::Core::Features::CMorphoFeature oMorphoFeature;			
                SS::Dictionary::Types::SWordInfo* pwi1 = &(it->front());

                m_pAMConverter->EnCode(
                    pwi1->m_WordIndex.m_DictIndex.GetDictionaryNumber(),
                    pwi1->m_MorphoTypeInfo.m_MorphoInfo,
                    pwi1->m_SpecialMorpho,
                    &oMorphoFeature);

                infoDetect.clear();
                if(m_pMorphoAnalyzer->GetWordFormsByIDSource(&(it->front().m_WordIndex.m_DictIndex), 
					                                         &(infoDetect)))
                {
                    for(std::list<SS::Dictionary::Types::TWordInfo>::iterator itWord = infoDetect.begin();
                        (form == "") && (itWord != infoDetect.end());
                        ++itWord)
                    {
                        pwi1 = &(itWord->front());

                        m_pAMConverter->EnCode(
                            pwi1->m_WordIndex.m_DictIndex.GetDictionaryNumber(),
                            pwi1->m_MorphoTypeInfo.m_MorphoInfo,
                            pwi1->m_SpecialMorpho,
                            &oMorphoFeature);

                        if(oMorphoFeature.m_CaseType.Equal(caseType) &&
                           oMorphoFeature.m_GenderType.Equal(gender) &&
                           oMorphoFeature.m_NumberType.Equal(Values::NumberType.ntSingle))
                        {
                            form = (itWord->front()).m_wWord.c_str();
                            result = result->Concat(result, form);
                            result = result->Concat(result, " ");
                        }
                    }
                }
            }
        }
    }
    if(result == "")
        result = word;
    return result->Trim();
}

String* CFullname::PersonToString(
    XElement* node,
    Types::CaseType caseType,
    Types::GenderType gender)
{
    using namespace System::Collections::Generic;
    StringBuilder* result = new StringBuilder();
    if(!TryGetFullname(node, result))
    {
        AppendWord(result, PrefixAndProffesion(node, caseType, gender));

        String* link = SemanticResolution::ServiceFunction::GetAttributeValue(node, __box(BlockAttribute::EI)->ToString());
        if(link != "")
        {
            XmlNode* lXmlNode = 0;
            m_Dictionary->TryGetValue(link, &lXmlNode);
            XElement* linkNode = SemanticResolution::ServiceFunction::ToXElement(lXmlNode);

            List<XElement*>* list = Enumerable::ToList(linkNode->Elements());
            for(int i = 0; i < list->Count; ++i)
            {
                if(list->Item[i]->Name->ToString()->Equals(__box(Lingvistic::NodeName::ENTITY)->ToString()))
                {
                    AppendWord(result, ToString(list->Item[i], 
                        __box(Case::Genetive)->ToString()->Substring(0, 1)));
                }
                else if(list->Item[i]->Value != "")
                    AppendWord(result, list->Item[i]->Value->Trim());
            }
        }

        String* firstName = SemanticResolutionController::GetServiceFunction()->GetAttributeValue(node, 
            __box(PersonAttribute::FN)->ToString());
        firstName = GetWordForm(firstName, caseType, gender);
        String* lastName = SemanticResolutionController::GetServiceFunction()->GetAttributeValue(node, 
            __box(PersonAttribute::LN)->ToString());
        lastName = GetWordForm(lastName, caseType, gender);
        String* middleName = SemanticResolutionController::GetServiceFunction()->GetAttributeValue(node, 
            __box(PersonAttribute::MN)->ToString());
        middleName = GetWordForm(middleName, caseType, gender);

        AppendWord(result, firstName);
        AppendWord(result, middleName);
        AppendWord(result, lastName);
    }
    return result->ToString()->Trim();
}

String* CFullname::DefaultToString(
    XElement* node,
    Types::CaseType caseType,
    Types::GenderType gender)
{
    using namespace System::Collections::Generic;
    StringBuilder* result = new StringBuilder();
    if(!TryGetFullname(node, result))
    {
        AppendWord(result, PrefixAndProffesion(node, caseType, gender));

        List<XElement*>* list = Enumerable::ToList(node->Elements());
        for(int i = 0; i < list->Count; ++i)
        {
            if(list->Item[i]->Name->ToString()->Equals(__box(Lingvistic::NodeName::ENTITY)->ToString()))
            {
                AppendWord(result, ToString(list->Item[i], 
                    __box(Case::Genetive)->ToString()->Substring(0, 1)));
            }
            else if(list->Item[i]->Value != "")
                AppendWord(result, list->Item[i]->Value->Trim());
        }
    }
    else if(!caseType.Equal(Values::CaseType.ctNominative))
    {
        StringBuilder* text = new StringBuilder();
        String* value = GetWordForm(result->ToString(), caseType, gender);
        text->Append(value);
        result = text;
    }
    return result->ToString()->Trim();
}

String* CFullname::PrefixAndProffesion(
    XElement* node,
    Types::CaseType caseType,
    Types::GenderType gender)
{
    StringBuilder* result = new StringBuilder();
    String* prof = SemanticResolutionController::GetServiceFunction()->GetAttributeValue(node, 
        __box(BlockAttribute::PROF)->ToString());
    prof = GetWordForm(prof, caseType, gender);
    String* prefix = SemanticResolutionController::GetServiceFunction()->GetAttributeValue(node, 
        __box(BlockAttribute::PRE)->ToString());
    prefix = GetWordForm(prefix, caseType, gender);
    
    AppendWord(result, prefix);
    AppendWord(result, prof);
    return result->ToString()->Trim();
}

void CFullname::AppendWord(StringBuilder* text, String* word)
{
    if(word != "")
    {
        text->Append(word);
        text->Append(" ");
    }
}

bool CFullname::TryGetFullname(XElement* node, StringBuilder* text)
{
    bool success = false;
    String* link = SemanticResolutionController::GetServiceFunction()->GetAttributeValue(node, 
        __box(BlockAttribute::LINK)->ToString());
    if(link != "")
    {
        XmlNode* lXmlNode = 0;
        m_Dictionary->TryGetValue(link, &lXmlNode);
        XElement* linkNode = SemanticResolutionController::GetServiceFunction()->ToXElement(lXmlNode);
        if(SemanticResolutionController::GetServiceFunction()->IsContainAttribute(linkNode, 
            __box(BlockAttribute::FULLNAME)->ToString()))
        {
            String* value = SemanticResolutionController::GetServiceFunction()->GetAttributeValue(linkNode, 
                __box(BlockAttribute::FULLNAME)->ToString());
            if(value == "")
                value = ToString(linkNode, __box(Case::Nominative)->ToString()->Substring(0, 1));

            success = true;
            text->Append(value);
        }
        else
        {
            success = true;
            text->Append(SemanticResolutionController::GetServiceFunction()->GetAttributeValue(linkNode, 
                __box(BlockAttribute::VALUE)->ToString()));
        }
    }
    return success;
}