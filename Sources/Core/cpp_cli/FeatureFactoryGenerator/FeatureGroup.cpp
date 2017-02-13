#include "StdAfx.h"
#include ".\featuregroup.h"
#using <mscorlib.dll>

namespace FeatureFactoryGenerator
{
	CFeatureGroup::CFeatureGroup(System::String* sGroupName)
	{
		m_sGroupName = sGroupName;
		m_FeatureArray = new System::Collections::ArrayList();
	}

	CFeatureGroup::~CFeatureGroup(void)
	{
	}

	///загружает характеристики из файлa в директории sPath
	void CFeatureGroup::LoadFeatures(System::String* sPath)
	{
		System::String* sText;
		System::String* sFileName = sPath->Concat(sPath, new System::String("\\"));
		sFileName = sFileName->Concat(sFileName, m_sGroupName);
		sFileName = sFileName->Concat(sFileName, new System::String(".ffgg"));

		System::IO::StreamReader* oSR = new System::IO::StreamReader(sFileName, System::Text::Encoding::GetEncoding(1251));
		sText = oSR->ReadToEnd();
		oSR->Close();
		sText->Replace ("	", " ");

		m_FeatureArray->Clear();

		int i = sText->IndexOf("[");
		while(i >= 0){
			System::String* sFeatureName = "";
			System::String* sFeatureDescription = "";
			System::String* sFeatureType = "";
			System::String* sFeatureTypeDescription = "";

			int j = sText->IndexOf("[", i + 1);
			System::String* sFeature;
			if(j == -1){
				sFeature = sText->Substring(i, sText->Length - i);
			}
			else{
				sFeature = sText->Substring(i, j - i);
			}

			//теперь парсим хар-ку и заносим ее в список
			int nSpace = 0;
			int nNameBeg = 1;
			int nNameEnd = sFeature->IndexOf(" ");

			//вначале пропускаем стартовые пробелы и ищем имя хар-ки
			while(nNameEnd - nSpace == 1){
				nSpace = nNameEnd;
				nNameBeg++;
				nNameEnd = sFeature->IndexOf(" ", nSpace + 1);
			}

			if(nNameEnd == -1) return;

			sFeatureName = sFeature->Substring(nNameBeg, nNameEnd - nNameBeg);

			//теперь ищем описание хар-ки
			nNameBeg = sFeature->IndexOf("\"",nNameEnd);
			nNameEnd = sFeature->IndexOf("\"",nNameBeg + 1);
			sFeatureDescription = sFeature->Substring(nNameBeg + 1, nNameEnd - nNameBeg - 1);

			//теперь ищем тип хар-ки
			nNameBeg = sFeature->IndexOf("]",nNameEnd);
			nNameEnd = sFeature->IndexOf("\n",nNameBeg);
			sFeatureType = sFeature->Substring(nNameBeg + 1, nNameEnd - nNameBeg - 1);
			sFeatureType = sFeatureType->Trim();

			if(sFeatureType->Length == 0){
				sFeatureType = "Map";
			}
			else if(sFeatureType->Equals("Map") == false){
				//тогда ищем описание типа
				nNameEnd = sFeatureType->IndexOf(" ");
				//sFeatureType = sFeatureType->Trim();

				nNameBeg = sFeatureType->IndexOf("\"",nNameEnd);
				if(nNameBeg != -1){

					nNameEnd = sFeatureType->IndexOf("\"",nNameBeg + 1);
					sFeatureTypeDescription = sFeatureType->Substring(nNameBeg + 1, nNameEnd - nNameBeg - 1);
					sFeatureTypeDescription = sFeatureTypeDescription->Trim();

					sFeatureType = sFeatureType->Substring(0, nNameBeg);
				}
				else{
					sFeatureType = sFeatureType->Substring(0, nNameEnd);
				}

				sFeatureType = sFeatureType->Trim();

			}

			nNameBeg = sFeature->IndexOf("\n");
			if(nNameBeg != -1){
				sFeature = sFeature->Substring(nNameBeg, sFeature->Length - nNameBeg)->Trim();
			}

			AddFeature(sFeatureName, sFeatureDescription, sFeatureType, sFeatureTypeDescription)->LoadValues(sFeature);

			i = j;
		}
	}

	///сохраняет характеристики в файл в директории sPath
	void CFeatureGroup::SaveFeatures(System::String* sPath)
	{
		System::String* sText;
		System::String* sFileName = sPath->Concat(sPath, new System::String("\\"));
		sFileName = sFileName->Concat(sFileName, m_sGroupName);
		sFileName = sFileName->Concat(sFileName, new System::String(".ffgg"));

		System::IO::StreamWriter* oSW = new System::IO::StreamWriter(sFileName, false, System::Text::Encoding::GetEncoding(1251));

		CFeature* pF = First();
		while(pF){
			sText = pF->Write(sText);
			pF = Next();
		}

		oSW->Write(sText);
		oSW->Close();
	}


	///добавляет новую характеристику
	CFeature* CFeatureGroup::AddFeature(
		System::String* sFeatureName
		, System::String* sFeatureDescription
		, System::String* sFeatureType
		, System::String* sFeatureTypeDescription)
	{
		CFeature* pF = new CFeature(sFeatureName, sFeatureDescription, sFeatureType, sFeatureTypeDescription);
		m_FeatureArray->Add(pF);

		return pF;
	}

	///удалит характеристику  под определенным номером
	void CFeatureGroup::DeleteFeature(unsigned int nFeature)
	{
		if(nFeature < m_FeatureArray->Count){
			m_FeatureArray->RemoveAt(nFeature);
		}
	}
	///вернет указатель на характеристику 
	CFeature* CFeatureGroup::GetFeature(unsigned int nFeature)
	{
		if(nFeature < m_FeatureArray->Count){
			return __try_cast<CFeature*>(m_FeatureArray->Item[nFeature]);
		}
		else{
			return NULL;
		}
	}
	///вернет указатель на первую характеристику 
	CFeature* CFeatureGroup::First()
	{
		m_ArrayEnum = m_FeatureArray->GetEnumerator();
		if(m_ArrayEnum->MoveNext()){
			return __try_cast<CFeature*>(m_ArrayEnum->Current);
		}
		else{
			return NULL;
		}
	}
	///вернет указатель на очередную характеристику 
	CFeature* CFeatureGroup::Next()
	{
		if(m_ArrayEnum->MoveNext()){
			return __try_cast<CFeature*>(m_ArrayEnum->Current);
		}
		else{
			return NULL;
		}
	}

}