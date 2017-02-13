#include "StdAfx.h"
#include ".\treegenerator.h"
#using <mscorlib.dll>
#include "..\..\cpp\ASSInterface\TDictionaryManager.h"
#include "..\..\cpp\ASSInterface\Iterators.h"

namespace LinguisticsKernel
{
CTreeGenerator::CTreeGenerator(SS::Interface::Core::ResourceManagers::ILoadManager* pLoadManager)
{
	m_pStringService = gcnew CStringService();
	m_pAMConverter = 
		(SS::Interface::Core::AMConverter::IAMConverterMorpho*)
		pLoadManager->GetInterface(L"./core/linguistics/AMConverter.dll", CLSID_AMConverterMorpho, IID_AMConverterMorpho);
}

CTreeGenerator::~CTreeGenerator(void)
{
}
///отрисует дерево по IText
void CTreeGenerator::GenerateTree(SS::Interface::Core::BlackBox::IText* pText, System::Windows::Forms::TreeView^ pTree, bool bIsSetObligatory)
{
	if(!pTree) return;
	pTree->Nodes->Clear();
	if(pText == NULL)
	{
		return;
	}
	SS::Interface::Core::BlackBox::IUnit* pUnit;

	System::UInt32 nPar = 0;
	System::UInt32 nSenTotal = 0;
	System::UInt32 nUnitTotal = 0;

	for(SS::Interface::Core::BlackBox::IParagraph* pPar = pText->GetFirstParagraph(); pPar; pPar = pPar->GetNextParagraph())
	{
		System::Windows::Forms::TreeNode^ pTNPar;
		System::UInt32 nSen = 0;
		pTNPar = pTree->Nodes->Add("");
		
		
		for(SS::Interface::Core::BlackBox::ISentence* pSen = pPar->GetFirstSentence(); pSen;	pSen = pSen->GetNextSentence())
		{
			System::Windows::Forms::TreeNode^ pTNSen;
			System::UInt32 nUnit = 0;

			pTNSen = pTNPar->Nodes->Add("");


			//for(SS::Iterators::CLeafIterator oLeafIterator(pSen->GetFirstUnit()); (*oLeafIterator) ;oLeafIterator++ )
			//{
			//	System::Windows::Forms::TreeNode^ pTNUnit;
			//	pTNUnit = pTNSen->Nodes->Add("");
			//	nUnit++;

			//	ShowUnit((*oLeafIterator), pTNUnit, nUnit, bIsSetObligatory);
			//}

			pUnit = pSen->GetFirstUnit();
			while(pUnit)
			{
				System::Windows::Forms::TreeNode^ pTNUnit;
				pTNUnit = pTNSen->Nodes->Add("");

				nUnit++;

				ShowUnit(pUnit, pTNUnit, nUnit, bIsSetObligatory);

				pUnit = pUnit->GetRightUnit();
			}

			nSen++;
			pTNSen->Text = System::String::Concat(gcnew System::String("[Предложение № "), nSen.ToString(), gcnew System::String("]"));
			nSenTotal++;			
		}
		nPar++;
		pTNPar->Text = System::String::Concat(gcnew System::String("[Параграф № "), nPar.ToString(), gcnew System::String("] Предложений: "), nSen.ToString());
	};
}


void CTreeGenerator::GenerateTree1(SS::Interface::Core::BlackBox::IText* pText, System::Windows::Forms::TreeView^ pTree, bool bIsSetObligatory)
{
	if(!pTree) return;
	pTree->Nodes->Clear();
	if(pText == NULL)
	{
		return;
	}
	SS::Interface::Core::BlackBox::IUnit* pUnit;

	System::UInt32 nPar = 0;
	System::UInt32 nSenTotal = 0;
	System::UInt32 nUnitTotal = 0;

	for(SS::Interface::Core::BlackBox::IParagraph* pPar = pText->GetFirstParagraph(); pPar; pPar = pPar->GetNextParagraph())
	{
		System::UInt32 nSen = 0;
		//pTNPar = pTree->Nodes->Add("");
		
		
		for(SS::Interface::Core::BlackBox::ISentence* pSen = pPar->GetFirstSentence(); pSen;	pSen = pSen->GetNextSentence())
		{
			System::UInt32 nUnit = 0;
			
			int i = pSen->isEnabled();
			System::String^ str =i.ToString();
			pUnit = pSen->GetFirstUnit();
			pTree->Nodes->Add(str);
		}
	};
}

///отрисует дерево по IQuery
void CTreeGenerator::GenerateTree(SS::Interface::Core::BlackBox::Query::IQuery* pQuery, System::Windows::Forms::TreeView^ pTree)
{
	SS_TRY
	{
		if(!pQuery)
			return;

		if(!pTree) return;
		pTree->Nodes->Clear();
		if(pQuery == NULL)
		{
			//SS_THROW(L"pQuery == NULL");
			return;
		}
		
		SS::Interface::Core::BlackBox::Query::ITransformation* pTransformation;
		SS::Interface::Core::BlackBox::Query::IGroup* pGroup;
		SS::Interface::Core::BlackBox::Query::IQueryUnit* pQueryUnit;
		SS::Interface::Core::BlackBox::Query::IQueryUnitIndex* pQueryUnitIndex;
		SS::Interface::Core::BlackBox::Query::IQueryUnitIndexCollection* pQueryUnitIndexCollection;

		System::Windows::Forms::TreeNode^ pTNTransformation = nullptr;
		System::Windows::Forms::TreeNode^ pTNGroup = nullptr;
		System::Windows::Forms::TreeNode^ pTNQueryUnit = nullptr;
		System::Windows::Forms::TreeNode^ pTNQueryUnitIndex = nullptr;
		System::Windows::Forms::TreeNode^ pTNQueryUnitIndexCollection = nullptr;

		
		//SS::Interface::Core::BlackBox::Query::ITransformation* pTransformation1 = pTransformation->GetNextTransformation();
		
		for(pTransformation = pQuery->GetFirstTransformation(); pTransformation; pTransformation = pTransformation->GetNextTransformation())
		{
			pTNTransformation = pTree->Nodes->Add("[Transformation]");
			for(pGroup = pTransformation->GetFirstGroup(); pGroup; pGroup = pGroup->GetNextGroup())
			{
				unsigned int seq_id = pGroup->GetSequenceID();
				//---UNUSED--- float weight = pGroup->GetWeight();
				BOOL b_oblG = pGroup->IsObligatory();

				System::String^ ss = gcnew System::String("[Group] ");
				ss = ss->Concat(ss, "SequenceId = ", seq_id.ToString());
				//ss = ss->Concat(ss, "; Weight = ", weight.ToString());
				if(b_oblG) 
					ss = ss->Concat(ss, "; IsObligatory");
				else
					ss = ss->Concat(ss, "; Not Obligatory");

				pTNGroup = pTNTransformation->Nodes->Add(ss);
				
				for(pQueryUnit = pGroup->GetFirstQueryUnit(); pQueryUnit; pQueryUnit = pQueryUnit->GetNextQueryUnit())
				{
					unsigned int seq_id = pQueryUnit->GetSequenceID();
					float weight = pQueryUnit->GetWeight();
					BOOL b_oblU = pQueryUnit->IsObligatory();
					System::String^ sss = gcnew System::String("[Unit] ");

					sss = sss->Concat(sss, "\"", gcnew System::String(pQueryUnit->GetWord()), "\" ");
					sss = sss->Concat(sss, "SequenceId = ", seq_id.ToString(), ";");
					sss = sss->Concat(sss, "<", weight.ToString(), ">");
					if(b_oblU) 
						sss = sss->Concat(sss, " Obligatory");
					else
						sss = sss->Concat(sss, " Not Obligatory");

					pTNQueryUnit = pTNGroup->Nodes->Add(sss);					
					pTNQueryUnit->NodeFont = gcnew System::Drawing::Font(pTNQueryUnit->TreeView->Font,System::Drawing::FontStyle::Bold);
					pTNQueryUnit->ForeColor = System::Drawing::Color::DarkSlateBlue;
				
					for(pQueryUnitIndexCollection = pQueryUnit->GetFirstQueryUnitIndexCollection(); pQueryUnitIndexCollection; pQueryUnitIndexCollection = pQueryUnitIndexCollection->GetNextQueryUnitIndexCollection())
					{
						//---UNUSED--- float weight = pQueryUnitIndexCollection->GetWeight();
						BOOL b_oblI = pQueryUnitIndexCollection->IsObligatory();
						System::String^ sic = gcnew System::String("[IndexCollection] ");
						sic = sic->Concat(sic, m_pStringService->IndexTypeToString(pQueryUnitIndexCollection->GetIndexCollectionType()));
						//sic = sic->Concat(sic, "; Weight = ", weight.ToString());
						//if(b_oblI) sic = sic->Concat(sic, "; IsObligatory");

						pTNQueryUnitIndexCollection = pTNQueryUnit->Nodes->Add(sic);
						pTNQueryUnitIndexCollection->ForeColor = System::Drawing::Color::Crimson;
						for(pQueryUnitIndex = pQueryUnitIndexCollection->GetFirstQueryUnitIndex(); pQueryUnitIndex; pQueryUnitIndex = pQueryUnitIndex->GetNextQueryUnitIndex())
						{
							unsigned int ui = pQueryUnitIndex->GetIndex().GetDictionaryIndex();

							if(pQueryUnitIndexCollection->GetIndexCollectionType() == SS::Interface::Core::BlackBox::eitSyntaxIndex)
							{
								pTNQueryUnitIndex = pTNQueryUnitIndexCollection->Nodes->Add(m_pStringService->SyntaxIndexToString(ui));
							}
							else if(pQueryUnitIndexCollection->GetIndexCollectionType() == SS::Interface::Core::BlackBox::eitSemanticIndex)
							{
								pTNQueryUnitIndex = pTNQueryUnitIndexCollection->Nodes->Add(m_pStringService->SemanticIndexToString(ui));
							}
							else if(pQueryUnitIndexCollection->GetIndexCollectionType() == SS::Interface::Core::BlackBox::eitMorphoIndex)
							{
								pTNQueryUnitIndex = pTNQueryUnitIndexCollection->Nodes->Add(m_pStringService->MorphoIndexToString(ui));
							}
							else if(pQueryUnitIndexCollection->GetIndexCollectionType() == SS::Interface::Core::BlackBox::eitSynonymIndex)
							{
								pTNQueryUnitIndex = pTNQueryUnitIndexCollection->Nodes->Add(m_pStringService->SynonymIndexToString(ui));
							}
							else
							{
								pTNQueryUnitIndex = pTNQueryUnitIndexCollection->Nodes->Add(ui.ToString());
							}							
						}						
					}					
				}				
			}			
		}
	}
	SS_CATCH(L"")
}

///отрисует юнит
void CTreeGenerator::ShowUnit(SS::Interface::Core::BlackBox::IUnit* pUnit, System::Windows::Forms::TreeNode^ pTNUnit, System::UInt32 nUnit, bool bIsSetObligatory)
{
	SS_TRY
	using namespace SS::Interface::Core::BlackBox;
	SS::Interface::Core::BlackBox::IUnit* pChildUnit;

	pTNUnit->Text = System::String::Concat(
		gcnew System::String(L"[Юнит № "), 
		nUnit.ToString(),  
		gcnew System::String(L"] "));

	float f = pUnit->GetWeight();

	pTNUnit->Text = pTNUnit->Text->Concat(
		pTNUnit->Text, 
		gcnew System::String(L" \""), 
		gcnew System::String(pUnit->GetWord()), 
		gcnew System::String(L"\""),
		gcnew System::String(L" <"),
		f.ToString(),
		gcnew System::String(L"> ")
		);
	if(bIsSetObligatory)
	{
		if(pUnit->IsObligatory())
			pTNUnit->Text = pTNUnit->Text->Concat(pTNUnit->Text, L"IsObligatory");
		else
			pTNUnit->Text = pTNUnit->Text->Concat(pTNUnit->Text, L"Not IsObligatory");
	}

	pTNUnit->NodeFont = gcnew System::Drawing::Font(pTNUnit->TreeView->Font,System::Drawing::FontStyle::Bold);

	pChildUnit = pUnit->GetLeftChildUnit();
	System::UInt32 nChildUnit = 0;
	while(pChildUnit)
	{
		System::Windows::Forms::TreeNode^ pTNChildUnit;
		pTNChildUnit = pTNUnit->Nodes->Add("");

		nChildUnit++;
		ShowUnit(pChildUnit, pTNChildUnit, nChildUnit, bIsSetObligatory);
		

		pChildUnit = pChildUnit->GetRightUnit();
	}

	//теперь дорисовываем индексы
	SS::Interface::Core::BlackBox::IIndex* pIndex;
	SS::Interface::Core::BlackBox::IDictionaryIndex* pDictionaryIndex;

	System::Windows::Forms::TreeNode^ pTNMorpho			= pTNUnit->Nodes->Add	("Морфология:");
	System::Windows::Forms::TreeNode^ pTNSyntax			= pTNUnit->Nodes->Add	("Синтакс:");
	System::Windows::Forms::TreeNode^ pTNSemantic		= pTNUnit->Nodes->Add	("Семантика:");
	System::Windows::Forms::TreeNode^ pTNSynonym		= pTNUnit->Nodes->Add	("Синонимы:");
	System::Windows::Forms::TreeNode^ pTNCombinatory	= nullptr;
	System::Windows::Forms::TreeNode^ pTNUnknown		= nullptr;
	System::String^ sSyntax = ""; 

	for(pIndex = pUnit->GetFirstIndex(); pIndex; pIndex = pUnit->GetNextIndex())
	{

		pDictionaryIndex = pIndex->GetDictionaryIndex();

		if(pDictionaryIndex)
		{
			SS::Interface::Core::BlackBox::EIndexType oIndexType;

			SS::Dictionary::Types::TDictionaryIndex oDictionaryIndex = pDictionaryIndex->GetFirst();
			oIndexType = oDictionaryIndex.GetIndexType();			

			//теперь дописываем индекс
			System::String^ s = "";

			//Вначале выбираем ветку, в которую дописываем индекс
			System::Windows::Forms::TreeNode^ pNode;
			switch(oIndexType)
			{
				//морфологический индекс
				case SS::Interface::Core::BlackBox::eitMorphoIndex:
					pNode = pTNMorpho;
					s = s->Concat(s, m_pStringService->MorphoIndexToShortString(oDictionaryIndex.GetDictionaryIndexWithoutFlags()));
					break;
				//синтаксический индекс
				case SS::Interface::Core::BlackBox::eitSyntaxIndex: 
					pNode = pTNSyntax;
					s = s->Concat(s, m_pStringService->SyntaxIndexToString(oDictionaryIndex.GetDictionaryIndexWithoutFlags()));
					break;
				//семантический индекс
				case SS::Interface::Core::BlackBox::eitSemanticIndex: 
					pNode = pTNSemantic;
					s = s->Concat(s, m_pStringService->SemanticIndexToString(oDictionaryIndex.GetDictionaryIndex()));
					break;
				//индекс синонима
				case SS::Interface::Core::BlackBox::eitSynonymIndex:
					pNode = pTNSynonym;
					s = s->Concat(s, m_pStringService->SynonymIndexToString(oDictionaryIndex.GetDictionaryIndexWithoutFlags()));
					break;
				//индекс комбинаторики
				case SS::Interface::Core::BlackBox::eitCombinatoryIndex:
					if(!pTNCombinatory) 
						pTNCombinatory = pTNUnit->Nodes->Add ("Комбинаторика: ");
					pNode = pTNCombinatory;
					s = s->Concat(s, (oDictionaryIndex.GetDictionaryIndexWithoutFlags()).ToString());
					break;
				//неопределенный индекс
				default: 
					if(!pTNUnknown) 
						pTNUnknown = pTNUnit->Nodes->Add("Unknown: ");
					pNode = pTNUnknown;
					s = s->Concat(s, (oDictionaryIndex.GetDictionaryIndexWithoutFlags()).ToString());
					break;
			}

			System::Windows::Forms::TreeNode^ pNodeIndex = pNode->Nodes->Add(s);

			for(IFeature * pFeature = pIndex->GetFirstFeature();pFeature; pFeature = pIndex->GetNextFeature())
			{
				SS::Core::Features::CMorphoFeature oMorphoFeature;
				m_pAMConverter->EnCode(oDictionaryIndex.GetDictionaryNumber(), 
					pFeature->GetMorphoInfo(),
					pFeature->GetPartOfSpeech(), &oMorphoFeature);
				System::Windows::Forms::TreeNode^ pNodeFeature = pNodeIndex->Nodes->Add( gcnew System::String(oMorphoFeature.GetPrintableDataDescription().c_str()) );
			}

		}
	}

	SS_CATCH(L"")
}
}