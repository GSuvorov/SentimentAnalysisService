#include "StdAfx.h"
#include "FeatureEditor.h"
#include ".\codegenerator.h"

namespace FeatureFactoryGenerator
{

	void FeatureEditor::InitializeMy()
	{
		comboFeatureType->SelectedIndex = 0;
		folderPath->SelectedPath = Environment::CurrentDirectory;
		labelPath->Text = folderPath->SelectedPath;

		m_pFeatureStorage = new CFeatureStorage();

		//здесь мы заполняем указатели на элементы интерфейса
		checkValues = new System::Windows::Forms::CheckBox* [32];
		textValueNames = new System::Windows::Forms::TextBox* [32];
		textValueDescs = new System::Windows::Forms::TextBox* [32];
		btnValues = new System::Windows::Forms::Button* [32];

		m_pCurrentFeature = NULL;

		btnValues[0] = btnValue0;
		btnValues[1] = btnValue1;
		btnValues[2] = btnValue2;
		btnValues[3] = btnValue3;
		btnValues[4] = btnValue4;
		btnValues[5] = btnValue5;
		btnValues[6] = btnValue6;
		btnValues[7] = btnValue7;
		btnValues[8] = btnValue8;
		btnValues[9] = btnValue9;

		btnValues[10] = btnValue10;
		btnValues[11] = btnValue11;
		btnValues[12] = btnValue12;
		btnValues[13] = btnValue13;
		btnValues[14] = btnValue14;
		btnValues[15] = btnValue15;
		btnValues[16] = btnValue16;
		btnValues[17] = btnValue17;
		btnValues[18] = btnValue18;
		btnValues[19] = btnValue19;

		btnValues[20] = btnValue20;
		btnValues[21] = btnValue21;
		btnValues[22] = btnValue22;
		btnValues[23] = btnValue23;
		btnValues[24] = btnValue24;
		btnValues[25] = btnValue25;
		btnValues[26] = btnValue26;
		btnValues[27] = btnValue27;
		btnValues[28] = btnValue28;
		btnValues[29] = btnValue29;

		btnValues[30] = btnValue30;
		btnValues[31] = btnValue31;

		checkValues[0] = checkValue0;
		checkValues[1] = checkValue1;
		checkValues[2] = checkValue2;
		checkValues[3] = checkValue3;
		checkValues[4] = checkValue4;
		checkValues[5] = checkValue5;
		checkValues[6] = checkValue6;
		checkValues[7] = checkValue7;
		checkValues[8] = checkValue8;
		checkValues[9] = checkValue9;

		checkValues[10] = checkValue10;
		checkValues[11] = checkValue11;
		checkValues[12] = checkValue12;
		checkValues[13] = checkValue13;
		checkValues[14] = checkValue14;
		checkValues[15] = checkValue15;
		checkValues[16] = checkValue16;
		checkValues[17] = checkValue17;
		checkValues[18] = checkValue18;
		checkValues[19] = checkValue19;

		checkValues[20] = checkValue20;
		checkValues[21] = checkValue21;
		checkValues[22] = checkValue22;
		checkValues[23] = checkValue23;
		checkValues[24] = checkValue24;
		checkValues[25] = checkValue25;
		checkValues[26] = checkValue26;
		checkValues[27] = checkValue27;
		checkValues[28] = checkValue28;
		checkValues[29] = checkValue29;

		checkValues[30] = checkValue30;
		checkValues[31] = checkValue31;

		textValueDescs[0] = textValueDesc0;
		textValueDescs[1] = textValueDesc1;
		textValueDescs[2] = textValueDesc2;
		textValueDescs[3] = textValueDesc3;
		textValueDescs[4] = textValueDesc4;
		textValueDescs[5] = textValueDesc5;
		textValueDescs[6] = textValueDesc6;
		textValueDescs[7] = textValueDesc7;
		textValueDescs[8] = textValueDesc8;
		textValueDescs[9] = textValueDesc9;

		textValueDescs[10] = textValueDesc10;
		textValueDescs[11] = textValueDesc11;
		textValueDescs[12] = textValueDesc12;
		textValueDescs[13] = textValueDesc13;
		textValueDescs[14] = textValueDesc14;
		textValueDescs[15] = textValueDesc15;
		textValueDescs[16] = textValueDesc16;
		textValueDescs[17] = textValueDesc17;
		textValueDescs[18] = textValueDesc18;
		textValueDescs[19] = textValueDesc19;

		textValueDescs[20] = textValueDesc20;
		textValueDescs[21] = textValueDesc21;
		textValueDescs[22] = textValueDesc22;
		textValueDescs[23] = textValueDesc23;
		textValueDescs[24] = textValueDesc24;
		textValueDescs[25] = textValueDesc25;
		textValueDescs[26] = textValueDesc26;
		textValueDescs[27] = textValueDesc27;
		textValueDescs[28] = textValueDesc28;
		textValueDescs[29] = textValueDesc29;

		textValueDescs[30] = textValueDesc30;
		textValueDescs[31] = textValueDesc31;

		textValueNames[0] = textValueName0;
		textValueNames[1] = textValueName1;
		textValueNames[2] = textValueName2;
		textValueNames[3] = textValueName3;
		textValueNames[4] = textValueName4;
		textValueNames[5] = textValueName5;
		textValueNames[6] = textValueName6;
		textValueNames[7] = textValueName7;
		textValueNames[8] = textValueName8;
		textValueNames[9] = textValueName9;

		textValueNames[10] = textValueName10;
		textValueNames[11] = textValueName11;
		textValueNames[12] = textValueName12;
		textValueNames[13] = textValueName13;
		textValueNames[14] = textValueName14;
		textValueNames[15] = textValueName15;
		textValueNames[16] = textValueName16;
		textValueNames[17] = textValueName17;
		textValueNames[18] = textValueName18;
		textValueNames[19] = textValueName19;

		textValueNames[20] = textValueName20;
		textValueNames[21] = textValueName21;
		textValueNames[22] = textValueName22;
		textValueNames[23] = textValueName23;
		textValueNames[24] = textValueName24;
		textValueNames[25] = textValueName25;
		textValueNames[26] = textValueName26;
		textValueNames[27] = textValueName27;
		textValueNames[28] = textValueName28;
		textValueNames[29] = textValueName29;

		textValueNames[30] = textValueName30;
		textValueNames[31] = textValueName31;

		LoadFeatures();
	};

	///вносит изменения во внешний вид интерфейса после выбора другого типа характеристики
	void FeatureEditor::UpdateTypeChanging()
	{
		if(comboFeatureType->SelectedIndex == 0){
			//мап
			groupIntegerFeature->Enabled = false;
			groupMapFeature->Enabled = true;
		}
		else{
			//интегер
			groupIntegerFeature->Enabled = true;
			groupMapFeature->Enabled = false;
		}
	}
	///загружает с диска в память все характеристики из файлов ".ffgg" в выбранной директории
	void FeatureEditor::LoadFeatures()
	{
		LoadFeatureGroups();
		UpdateView();
	}
	///сохраняет на диск все характеристики из памяти в файлы ".ffgg" в выбранной директории
	void FeatureEditor::SaveFeatures()
	{
		m_pFeatureStorage->SaveGroups(labelPath->Text);
	}


	///обновит вид интерфейса по вновь загруженным характеристикам
	void FeatureEditor::UpdateView()
	{
		CFeatureGroup* pFG;
		pFG = m_pFeatureStorage->FirstGroup();
		comboFeatureGroup->Items->Clear();
		while(pFG){
			comboFeatureGroup->Items->Add(pFG->GetGroupName());
			pFG = m_pFeatureStorage->NextGroup();
		}
		if(comboFeatureGroup->Items->Count > 0)	comboFeatureGroup->SelectedIndex = 0;
	}
	///обновит вид списка характеристик по номеру группы
	void FeatureEditor::UpdateGroupView(unsigned int nGroup)
	{
		CFeatureGroup* pFG = m_pFeatureStorage->GetGroup(nGroup);

		listFeatures->Items->Clear();

		if(pFG){
			CFeature* pF;
			pF = pFG->First();

			while(pF){
				listFeatures->Items->Add(pF->GetFeatureName());
				pF = pFG->Next();
			}
		}

		UpdateFeatureView(comboFeatureGroup->SelectedIndex, listFeatures->SelectedIndex);
	}


	///ищет по указанному пути файлы ".ffgg" и формирует на их основе список групп характеристик
	void FeatureEditor::LoadFeatureGroups()
	{
		m_pFeatureStorage->LoadGroups(labelPath->Text);
		UpdateGroupView(comboFeatureGroup->SelectedIndex);
	}

	///обнуляет вид окна редактирования характеристики
	void FeatureEditor::ClearFeatureView()
	{
		textFeatureName->Text = "";
		textFeatureDescription->Text = "";
		textIntegerDescription->Text = "";

		//btnSaveFeature->Enabled = false;

		unsigned int i = 0;

		while(i < 32){
			textValueNames[i]->Text = "";
			textValueDescs[i]->Text = "";
			checkValues[i]->Checked = false;
			btnValues[i]->ImageIndex = 2;
			toolTip1->SetToolTip(btnValues[i], "");

			textValueNames[i]->Enabled = false;
			textValueDescs[i]->Enabled = false;
			checkValues[i]->Enabled = false;
			btnValues[i]->Enabled = false;

			textValueNames[i]->BackColor = System::Drawing::SystemColors::InactiveBorder;
			textValueDescs[i]->BackColor = System::Drawing::SystemColors::InactiveBorder;

			i++;
		}

	}

	///выведет инфу о хар-ке в окошко редактирования
	void FeatureEditor::UpdateFeatureView(unsigned int nGroup, unsigned int nFeature)
	{
		ClearFeatureView();

		if(listFeatures->SelectedIndex == -1){
			groupFeatureEditor->Enabled = false;
			return;
		}
		else{
			groupFeatureEditor->Enabled = true;
		}

		CFeatureGroup* pFG = m_pFeatureStorage->GetGroup(nGroup);

		if(pFG){
			CFeature* pF;
			pF = pFG->GetFeature(nFeature);

			if(pF){
				ShowFeature(pF);
			}
		}
		btnSaveFeature->Enabled = false;
	}

	///выведет инфу о хар-ке из окошко редактирования в характеристику
	void FeatureEditor::UpdateFeatureFromView(unsigned int nGroup, unsigned int nFeature)
	{
		CFeatureGroup* pFG = m_pFeatureStorage->GetGroup(nGroup);

		if(pFG){
			CFeature* pF;
			pF = pFG->GetFeature(nFeature);

			if(pF){
				WriteFeature(pF);
			}
		}
	}

	///отобразит в окне редактирования характеристику
	void FeatureEditor::ShowFeature(CFeature* pFeature)
	{
		if(pFeature == NULL) return;

		textFeatureName->Text = pFeature->GetFeatureName();
		textFeatureDescription->Text = pFeature->GetFeatureDescription();

		if(pFeature->GetFeatureType()->Equals("Map")){
			comboFeatureType->SelectedIndex = 0;
		}
		else if(pFeature->GetFeatureType()->Equals("Integer")){
			comboFeatureType->SelectedIndex = 1;
		}
		else if(pFeature->GetFeatureType()->Equals("Percent")){
			comboFeatureType->SelectedIndex = 2;
		}
		else{
			System::Windows::Forms::MessageBox::Show(
				"Ошибочный тип характеристики. Убедитесь, что формат записи правильный."
				, "Ошибка загрузки характеристики (bros)"
				, System::Windows::Forms::MessageBoxButtons::OK
				, System::Windows::Forms::MessageBoxIcon::Error);

			comboFeatureType->SelectedIndex = 0;
		}

		textIntegerDescription->Text = pFeature->GetFeatureTypeDescription();

		//теперь отображаем значения мап-характеристик
		CFeatureValue* pFV = pFeature->FirstValue();

		unsigned int i = 0;

		while(pFV){
			if(i > 31) throw new System::Exception("слишком много значений у характеристики");

			textValueNames[i]->Text = pFV->GetValueName();
			textValueDescs[i]->Text = pFV->GetValueDescription();
			checkValues[i]->Checked = pFV->IsEnabled();
			btnValues[i]->ImageIndex = 0;
			toolTip1->SetToolTip(btnValues[i], "Удалить");

			textValueNames[i]->Enabled = true;
			textValueDescs[i]->Enabled = true;
			checkValues[i]->Enabled = true;
			btnValues[i]->Enabled = true;

			textValueNames[i]->BackColor = System::Drawing::SystemColors::Info;
			textValueDescs[i]->BackColor = System::Drawing::SystemColors::Info;

			pFV = pFeature->NextValue();
			i++;
		}

		if(i < 32){
			btnValues[i]->ImageIndex = 1;
			btnValues[i]->Enabled = true;
			toolTip1->SetToolTip(btnValues[i], "Добавить");
		}

		m_pCurrentFeature = pFeature;
	}

	///выведет инфу о хар-ке из окошко редактирования в характеристику
	void FeatureEditor::WriteFeature(CFeature* pFeature)
	{
		pFeature->SetFeatureName(textFeatureName->Text);
		pFeature->SetFeatureDescription(textFeatureDescription->Text);

		if(comboFeatureType->SelectedIndex == 0){
			pFeature->SetFeatureType("Map");
		}
		else if(comboFeatureType->SelectedIndex == 1){
			pFeature->SetFeatureType("Integer");
		}
		else if(comboFeatureType->SelectedIndex == 2){
			pFeature->SetFeatureType("Percent");
		}
		else{
			System::Windows::Forms::MessageBox::Show(
				"Ошибочный тип характеристики. Убедитесь, что формат записи правильный."
				, "Ошибка загрузки характеристики (bros)"
				, System::Windows::Forms::MessageBoxButtons::OK
				, System::Windows::Forms::MessageBoxIcon::Error);

			pFeature->SetFeatureType("Map");
		}

		pFeature->SetFeatureTypeDescription(textIntegerDescription->Text);

		//теперь отображаем значения мап-характеристик
		pFeature->ClearMapValues();

		unsigned int i = 0;

		while(i < 32){
			if(textValueNames[i]->Enabled){
				pFeature->AddMapValue(
					textValueNames[i]->Text
					, textValueDescs[i]->Text
					, checkValues[i]->Checked
					);
			}
			else{
				break;
			}

			i++;
		}
	}
	///добавить/удалить мап-значение характеристики
	void FeatureEditor::AddRemoveFeatureValue(unsigned int nValue)
	{
		System::Windows::Forms::Button* btn = btnValues[nValue];

//		CFeatureGroup* pFG = m_pFeatureStorage->GetGroup(comboFeatureGroup->SelectedIndex);
//		if(pFG){
			CFeature* pF;
			pF = m_pCurrentFeature;//pFG->GetFeature(listFeatures->SelectedIndex);

			if(pF){
				CFeature* pNF = new CFeature(pF);
				WriteFeature(pNF);

				if(btn->ImageIndex == 0){
					pNF->DeleteFeatureValue(nValue);
				}
				else if(btn->ImageIndex == 1){
					pNF->AddMapValue("name", "description", true);
				}
				ClearFeatureView();
				ShowFeature(pNF);
			}
//		}
	}

	///добавит пустую новую хар-ку в текущую группу
	void FeatureEditor::AddFeature()
	{
		CFeatureGroup* pFG = m_pFeatureStorage->GetGroup(comboFeatureGroup->SelectedIndex);

		if(pFG){
			CFeature* pF;
			pF = pFG->AddFeature("Name", "Описание", "Integer", "");

			if(pF){
				UpdateGroupView(comboFeatureGroup->SelectedIndex);
				listFeatures->SelectedIndex = listFeatures->Items->Count - 1;
			}
		}
	}

	///удалит выбранную хар-ку в текущей группе
	void FeatureEditor::DeleteFeature()
	{
		CFeatureGroup* pFG = m_pFeatureStorage->GetGroup(comboFeatureGroup->SelectedIndex);

		if(pFG){
			CFeature* pF;
			pF = pFG->GetFeature(listFeatures->SelectedIndex);

			if(pF){
				pFG->DeleteFeature(listFeatures->SelectedIndex);
				UpdateGroupView(comboFeatureGroup->SelectedIndex);
				int temp = m_SelectedFeature;
				if(temp < listFeatures->Items->Count){
				}
				else{
					temp = listFeatures->Items->Count - 1;
				}
				m_SelectedFeature = -1;
				listFeatures->SelectedIndex = temp;
			}
		}
	}

	///сгенерирует код С++
	void FeatureEditor::GenerateCode()
	{
		CCodeGenerator oCG;

		System::String* sText = oCG.Generate(m_pFeatureStorage);
		System::String* sFileName = labelPath->Text->Concat(labelPath->Text, new System::String("\\AutoFeatures.h"));

		System::IO::StreamWriter* oSW = new System::IO::StreamWriter(sFileName, false, System::Text::Encoding::GetEncoding(1251));

		oSW->Write(sText);
		oSW->Close();
			//System::Windows::Forms::MessageBox::Show(
			//	s
			//	, "code"
			//	, System::Windows::Forms::MessageBoxButtons::OK
			//	, System::Windows::Forms::MessageBoxIcon::None);
	}
}