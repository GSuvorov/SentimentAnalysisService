#pragma once

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;

#include "FeatureStorage.h"

namespace FeatureFactoryGenerator
{
	/// <summary> 
	/// Summary for FeatureEditor
	///
	/// WARNING: If you change the name of this class, you will need to change the 
	///          'Resource File Name' property for the managed resource compiler tool 
	///          associated with all .resx files this class depends on.  Otherwise,
	///          the designers will not be able to interact properly with localized
	///          resources associated with this form.
	/// </summary>
	public __gc class FeatureEditor : public System::Windows::Forms::Form
	{
	public: 
		FeatureEditor(void)
		{
			InitializeComponent();
			InitializeMy();
		}
        
	protected: 
		void Dispose(Boolean disposing)
		{
			if (disposing && components)
			{
				components->Dispose();
			}
			__super::Dispose(disposing);
		}




	private: System::Windows::Forms::Label *  label1;
	private: System::Windows::Forms::Label *  label2;
	private: System::Windows::Forms::LinkLabel *  linkPath;
	private: System::Windows::Forms::ComboBox *  comboFeatureGroup;
	private: System::Windows::Forms::ListBox *  listFeatures;
	private: System::Windows::Forms::Button *  btnAddFeature;
	private: System::Windows::Forms::Button *  btnDeleteFeature;

	private: System::Windows::Forms::Label *  labelPath;

	private: System::Windows::Forms::Label *  label3;
	private: System::Windows::Forms::Label *  label4;


	private: System::Windows::Forms::Label *  label5;
	private: System::Windows::Forms::FolderBrowserDialog *  folderPath;
	private: System::Windows::Forms::ComboBox *  comboFeatureType;
	private: System::Windows::Forms::TextBox *  textFeatureDescription;
	private: System::Windows::Forms::TextBox *  textFeatureName;


	private: System::Windows::Forms::Label *  label6;



































































private: System::Windows::Forms::ToolTip *  toolTip1;

































































private: System::Windows::Forms::GroupBox *  groupFeatureSelection;
private: System::Windows::Forms::GroupBox *  groupFeatureEditor;
private: System::Windows::Forms::Button *  btnSaveFeature;
private: System::Windows::Forms::GroupBox *  groupIntegerFeature;
private: System::Windows::Forms::TextBox *  textIntegerDescription;
private: System::Windows::Forms::GroupBox *  groupMapFeature;
private: System::Windows::Forms::Button *  btnExitNoSave;
private: System::Windows::Forms::Button *  btnExitAndSave;
private: System::ComponentModel::IContainer *  components;
private: System::Windows::Forms::TextBox *  textValueDesc14;
private: System::Windows::Forms::TextBox *  textValueDesc13;
private: System::Windows::Forms::TextBox *  textValueDesc12;
private: System::Windows::Forms::TextBox *  textValueDesc11;
private: System::Windows::Forms::TextBox *  textValueDesc10;
private: System::Windows::Forms::TextBox *  textValueDesc9;
private: System::Windows::Forms::TextBox *  textValueDesc8;
private: System::Windows::Forms::TextBox *  textValueDesc7;
private: System::Windows::Forms::TextBox *  textValueDesc6;
private: System::Windows::Forms::TextBox *  textValueDesc5;
private: System::Windows::Forms::TextBox *  textValueDesc4;
private: System::Windows::Forms::TextBox *  textValueDesc3;
private: System::Windows::Forms::TextBox *  textValueDesc2;
private: System::Windows::Forms::TextBox *  textValueDesc1;
private: System::Windows::Forms::TextBox *  textValueDesc0;
private: System::Windows::Forms::TextBox *  textValueName31;
private: System::Windows::Forms::TextBox *  textValueName30;
private: System::Windows::Forms::TextBox *  textValueName29;
private: System::Windows::Forms::TextBox *  textValueName28;
private: System::Windows::Forms::TextBox *  textValueName27;
private: System::Windows::Forms::TextBox *  textValueName26;
private: System::Windows::Forms::TextBox *  textValueName25;
private: System::Windows::Forms::TextBox *  textValueName24;
private: System::Windows::Forms::TextBox *  textValueName23;
private: System::Windows::Forms::TextBox *  textValueName22;
private: System::Windows::Forms::TextBox *  textValueName21;
private: System::Windows::Forms::TextBox *  textValueName20;
private: System::Windows::Forms::TextBox *  textValueName19;
private: System::Windows::Forms::TextBox *  textValueName18;
private: System::Windows::Forms::TextBox *  textValueName17;
private: System::Windows::Forms::TextBox *  textValueName16;
private: System::Windows::Forms::TextBox *  textValueName15;
private: System::Windows::Forms::TextBox *  textValueName14;
private: System::Windows::Forms::TextBox *  textValueName13;
private: System::Windows::Forms::TextBox *  textValueName12;
private: System::Windows::Forms::TextBox *  textValueName11;
private: System::Windows::Forms::TextBox *  textValueName10;
private: System::Windows::Forms::TextBox *  textValueName9;
private: System::Windows::Forms::TextBox *  textValueName8;
private: System::Windows::Forms::TextBox *  textValueName7;
private: System::Windows::Forms::TextBox *  textValueName6;
private: System::Windows::Forms::TextBox *  textValueName5;
private: System::Windows::Forms::TextBox *  textValueName4;
private: System::Windows::Forms::TextBox *  textValueName3;
private: System::Windows::Forms::TextBox *  textValueName2;
private: System::Windows::Forms::TextBox *  textValueName1;
private: System::Windows::Forms::TextBox *  textValueName0;
private: System::Windows::Forms::TextBox *  textValueDesc31;
private: System::Windows::Forms::TextBox *  textValueDesc30;
private: System::Windows::Forms::TextBox *  textValueDesc29;
private: System::Windows::Forms::TextBox *  textValueDesc28;
private: System::Windows::Forms::TextBox *  textValueDesc27;
private: System::Windows::Forms::TextBox *  textValueDesc26;
private: System::Windows::Forms::TextBox *  textValueDesc25;
private: System::Windows::Forms::TextBox *  textValueDesc24;
private: System::Windows::Forms::TextBox *  textValueDesc23;
private: System::Windows::Forms::TextBox *  textValueDesc22;
private: System::Windows::Forms::TextBox *  textValueDesc21;
private: System::Windows::Forms::TextBox *  textValueDesc20;
private: System::Windows::Forms::TextBox *  textValueDesc19;
private: System::Windows::Forms::TextBox *  textValueDesc18;
private: System::Windows::Forms::TextBox *  textValueDesc17;
private: System::Windows::Forms::TextBox *  textValueDesc16;
private: System::Windows::Forms::TextBox *  textValueDesc15;
private: System::Windows::Forms::CheckBox *  checkValue31;
private: System::Windows::Forms::CheckBox *  checkValue30;
private: System::Windows::Forms::CheckBox *  checkValue29;
private: System::Windows::Forms::CheckBox *  checkValue28;
private: System::Windows::Forms::CheckBox *  checkValue27;
private: System::Windows::Forms::CheckBox *  checkValue26;
private: System::Windows::Forms::CheckBox *  checkValue25;
private: System::Windows::Forms::CheckBox *  checkValue24;
private: System::Windows::Forms::CheckBox *  checkValue23;
private: System::Windows::Forms::CheckBox *  checkValue22;
private: System::Windows::Forms::CheckBox *  checkValue21;
private: System::Windows::Forms::CheckBox *  checkValue20;
private: System::Windows::Forms::CheckBox *  checkValue19;
private: System::Windows::Forms::CheckBox *  checkValue18;
private: System::Windows::Forms::CheckBox *  checkValue17;
private: System::Windows::Forms::CheckBox *  checkValue16;
private: System::Windows::Forms::CheckBox *  checkValue15;
private: System::Windows::Forms::CheckBox *  checkValue14;
private: System::Windows::Forms::CheckBox *  checkValue13;
private: System::Windows::Forms::CheckBox *  checkValue12;
private: System::Windows::Forms::CheckBox *  checkValue11;
private: System::Windows::Forms::CheckBox *  checkValue10;
private: System::Windows::Forms::CheckBox *  checkValue9;
private: System::Windows::Forms::CheckBox *  checkValue8;
private: System::Windows::Forms::CheckBox *  checkValue7;
private: System::Windows::Forms::CheckBox *  checkValue6;
private: System::Windows::Forms::CheckBox *  checkValue5;
private: System::Windows::Forms::CheckBox *  checkValue4;
private: System::Windows::Forms::CheckBox *  checkValue3;
private: System::Windows::Forms::CheckBox *  checkValue2;
private: System::Windows::Forms::CheckBox *  checkValue1;
private: System::Windows::Forms::CheckBox *  checkValue0;

		///набор указателей на чек-боксы используемых значений мап-характеристик
private: System::Windows::Forms::CheckBox *  checkValues[];
		///набор указателей на имена используемых значений мап-характеристик
private: System::Windows::Forms::TextBox *  textValueNames[];
		///набор указателей на описания используемых значений мап-характеристик
private: System::Windows::Forms::TextBox *  textValueDescs[];
		///набор указателей на кнопки используемых значений мап-характеристик
private: System::Windows::Forms::Button *  btnValues[];
		///хранилище всех загруженных/редактируемых характеристик
private: CFeatureStorage* m_pFeatureStorage;

		 ///отображенная в настоящий момент фича (это для поддержки доб/удал. значений)
		 CFeature* m_pCurrentFeature;


private: System::Windows::Forms::Button *  btnUndoChanges;

private: System::Windows::Forms::Button *  btnValue31;
private: System::Windows::Forms::Button *  btnValue30;
private: System::Windows::Forms::Button *  btnValue29;
private: System::Windows::Forms::Button *  btnValue28;
private: System::Windows::Forms::Button *  btnValue27;
private: System::Windows::Forms::Button *  btnValue26;
private: System::Windows::Forms::Button *  btnValue25;
private: System::Windows::Forms::Button *  btnValue24;
private: System::Windows::Forms::Button *  btnValue23;
private: System::Windows::Forms::Button *  btnValue22;
private: System::Windows::Forms::Button *  btnValue21;
private: System::Windows::Forms::Button *  btnValue20;
private: System::Windows::Forms::Button *  btnValue19;
private: System::Windows::Forms::Button *  btnValue18;
private: System::Windows::Forms::Button *  btnValue17;
private: System::Windows::Forms::Button *  btnValue16;
private: System::Windows::Forms::Button *  btnValue15;
private: System::Windows::Forms::Button *  btnValue14;
private: System::Windows::Forms::Button *  btnValue13;
private: System::Windows::Forms::Button *  btnValue12;
private: System::Windows::Forms::Button *  btnValue11;
private: System::Windows::Forms::Button *  btnValue10;
private: System::Windows::Forms::Button *  btnValue9;
private: System::Windows::Forms::Button *  btnValue8;
private: System::Windows::Forms::Button *  btnValue7;
private: System::Windows::Forms::Button *  btnValue6;
private: System::Windows::Forms::Button *  btnValue5;
private: System::Windows::Forms::Button *  btnValue4;
private: System::Windows::Forms::Button *  btnValue3;
private: System::Windows::Forms::Button *  btnValue2;
private: System::Windows::Forms::Button *  btnValue1;
private: System::Windows::Forms::Button *  btnValue0;
private: System::Windows::Forms::ImageList *  imageList1;
private: System::Windows::Forms::PictureBox *  pictureBox1;
private: System::Windows::Forms::Label *  label7;
private: System::Windows::Forms::Label *  label8;
private: System::Windows::Forms::Label *  label9;
private: System::Windows::Forms::Label *  label10;
private: System::Windows::Forms::Label *  label11;
private: System::Windows::Forms::Label *  label12;
private: System::Windows::Forms::Label *  label13;
private: System::Windows::Forms::Label *  label14;
private: System::Windows::Forms::Label *  label15;
private: System::Windows::Forms::Label *  label16;
private: System::Windows::Forms::Label *  label17;
private: System::Windows::Forms::Label *  label18;
private: System::Windows::Forms::Label *  label19;
private: System::Windows::Forms::Label *  label20;
private: System::Windows::Forms::Label *  label21;
private: System::Windows::Forms::Label *  label22;
private: System::Windows::Forms::Label *  label23;
private: System::Windows::Forms::Label *  label24;
private: System::Windows::Forms::Label *  label25;
private: System::Windows::Forms::Label *  label26;
private: System::Windows::Forms::Label *  label27;
private: System::Windows::Forms::Label *  label28;
private: System::Windows::Forms::Label *  label29;
private: System::Windows::Forms::Label *  label30;
private: System::Windows::Forms::Label *  label31;
private: System::Windows::Forms::Label *  label32;
private: System::Windows::Forms::Label *  label33;
private: System::Windows::Forms::Label *  label34;
private: System::Windows::Forms::Label *  label35;
private: System::Windows::Forms::Label *  label36;
private: System::Windows::Forms::Label *  label37;
private: System::Windows::Forms::Label *  label38;
private: System::Windows::Forms::GroupBox *  groupBox1;
private: System::Windows::Forms::Button *  btnGenerate;

		 ///запоминаем здесь выбранную хар-ку
private: unsigned int m_SelectedFeature;

	private:
		/// <summary>
		/// Required designer variable.
		/// </summary>


		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		void InitializeComponent(void)
		{
			this->components = new System::ComponentModel::Container();
			System::Resources::ResourceManager *  resources = new System::Resources::ResourceManager(__typeof(FeatureFactoryGenerator::FeatureEditor));
			this->groupFeatureSelection = new System::Windows::Forms::GroupBox();
			this->btnDeleteFeature = new System::Windows::Forms::Button();
			this->btnAddFeature = new System::Windows::Forms::Button();
			this->label2 = new System::Windows::Forms::Label();
			this->label1 = new System::Windows::Forms::Label();
			this->listFeatures = new System::Windows::Forms::ListBox();
			this->comboFeatureGroup = new System::Windows::Forms::ComboBox();
			this->linkPath = new System::Windows::Forms::LinkLabel();
			this->groupFeatureEditor = new System::Windows::Forms::GroupBox();
			this->btnUndoChanges = new System::Windows::Forms::Button();
			this->btnSaveFeature = new System::Windows::Forms::Button();
			this->groupIntegerFeature = new System::Windows::Forms::GroupBox();
			this->label6 = new System::Windows::Forms::Label();
			this->textIntegerDescription = new System::Windows::Forms::TextBox();
			this->groupMapFeature = new System::Windows::Forms::GroupBox();
			this->label36 = new System::Windows::Forms::Label();
			this->label37 = new System::Windows::Forms::Label();
			this->label38 = new System::Windows::Forms::Label();
			this->label31 = new System::Windows::Forms::Label();
			this->label32 = new System::Windows::Forms::Label();
			this->label33 = new System::Windows::Forms::Label();
			this->label34 = new System::Windows::Forms::Label();
			this->label35 = new System::Windows::Forms::Label();
			this->label26 = new System::Windows::Forms::Label();
			this->label27 = new System::Windows::Forms::Label();
			this->label28 = new System::Windows::Forms::Label();
			this->label29 = new System::Windows::Forms::Label();
			this->label30 = new System::Windows::Forms::Label();
			this->label18 = new System::Windows::Forms::Label();
			this->label19 = new System::Windows::Forms::Label();
			this->label20 = new System::Windows::Forms::Label();
			this->label21 = new System::Windows::Forms::Label();
			this->label22 = new System::Windows::Forms::Label();
			this->label23 = new System::Windows::Forms::Label();
			this->label24 = new System::Windows::Forms::Label();
			this->label25 = new System::Windows::Forms::Label();
			this->label14 = new System::Windows::Forms::Label();
			this->label15 = new System::Windows::Forms::Label();
			this->label16 = new System::Windows::Forms::Label();
			this->label17 = new System::Windows::Forms::Label();
			this->label13 = new System::Windows::Forms::Label();
			this->label12 = new System::Windows::Forms::Label();
			this->label11 = new System::Windows::Forms::Label();
			this->label10 = new System::Windows::Forms::Label();
			this->label9 = new System::Windows::Forms::Label();
			this->label8 = new System::Windows::Forms::Label();
			this->label7 = new System::Windows::Forms::Label();
			this->btnValue31 = new System::Windows::Forms::Button();
			this->imageList1 = new System::Windows::Forms::ImageList(this->components);
			this->checkValue31 = new System::Windows::Forms::CheckBox();
			this->btnValue30 = new System::Windows::Forms::Button();
			this->checkValue30 = new System::Windows::Forms::CheckBox();
			this->btnValue29 = new System::Windows::Forms::Button();
			this->checkValue29 = new System::Windows::Forms::CheckBox();
			this->btnValue28 = new System::Windows::Forms::Button();
			this->checkValue28 = new System::Windows::Forms::CheckBox();
			this->btnValue27 = new System::Windows::Forms::Button();
			this->checkValue27 = new System::Windows::Forms::CheckBox();
			this->btnValue26 = new System::Windows::Forms::Button();
			this->checkValue26 = new System::Windows::Forms::CheckBox();
			this->btnValue25 = new System::Windows::Forms::Button();
			this->checkValue25 = new System::Windows::Forms::CheckBox();
			this->btnValue24 = new System::Windows::Forms::Button();
			this->checkValue24 = new System::Windows::Forms::CheckBox();
			this->btnValue23 = new System::Windows::Forms::Button();
			this->checkValue23 = new System::Windows::Forms::CheckBox();
			this->btnValue22 = new System::Windows::Forms::Button();
			this->checkValue22 = new System::Windows::Forms::CheckBox();
			this->btnValue21 = new System::Windows::Forms::Button();
			this->checkValue21 = new System::Windows::Forms::CheckBox();
			this->btnValue20 = new System::Windows::Forms::Button();
			this->checkValue20 = new System::Windows::Forms::CheckBox();
			this->btnValue19 = new System::Windows::Forms::Button();
			this->checkValue19 = new System::Windows::Forms::CheckBox();
			this->btnValue18 = new System::Windows::Forms::Button();
			this->checkValue18 = new System::Windows::Forms::CheckBox();
			this->btnValue17 = new System::Windows::Forms::Button();
			this->checkValue17 = new System::Windows::Forms::CheckBox();
			this->btnValue16 = new System::Windows::Forms::Button();
			this->checkValue16 = new System::Windows::Forms::CheckBox();
			this->btnValue15 = new System::Windows::Forms::Button();
			this->checkValue15 = new System::Windows::Forms::CheckBox();
			this->btnValue14 = new System::Windows::Forms::Button();
			this->checkValue14 = new System::Windows::Forms::CheckBox();
			this->btnValue13 = new System::Windows::Forms::Button();
			this->checkValue13 = new System::Windows::Forms::CheckBox();
			this->btnValue12 = new System::Windows::Forms::Button();
			this->checkValue12 = new System::Windows::Forms::CheckBox();
			this->btnValue11 = new System::Windows::Forms::Button();
			this->checkValue11 = new System::Windows::Forms::CheckBox();
			this->btnValue10 = new System::Windows::Forms::Button();
			this->checkValue10 = new System::Windows::Forms::CheckBox();
			this->btnValue9 = new System::Windows::Forms::Button();
			this->checkValue9 = new System::Windows::Forms::CheckBox();
			this->btnValue8 = new System::Windows::Forms::Button();
			this->checkValue8 = new System::Windows::Forms::CheckBox();
			this->btnValue7 = new System::Windows::Forms::Button();
			this->checkValue7 = new System::Windows::Forms::CheckBox();
			this->btnValue6 = new System::Windows::Forms::Button();
			this->checkValue6 = new System::Windows::Forms::CheckBox();
			this->btnValue5 = new System::Windows::Forms::Button();
			this->checkValue5 = new System::Windows::Forms::CheckBox();
			this->btnValue4 = new System::Windows::Forms::Button();
			this->checkValue4 = new System::Windows::Forms::CheckBox();
			this->btnValue3 = new System::Windows::Forms::Button();
			this->checkValue3 = new System::Windows::Forms::CheckBox();
			this->btnValue2 = new System::Windows::Forms::Button();
			this->checkValue2 = new System::Windows::Forms::CheckBox();
			this->btnValue1 = new System::Windows::Forms::Button();
			this->checkValue1 = new System::Windows::Forms::CheckBox();
			this->btnValue0 = new System::Windows::Forms::Button();
			this->checkValue0 = new System::Windows::Forms::CheckBox();
			this->textValueDesc31 = new System::Windows::Forms::TextBox();
			this->textValueDesc30 = new System::Windows::Forms::TextBox();
			this->textValueDesc29 = new System::Windows::Forms::TextBox();
			this->textValueDesc28 = new System::Windows::Forms::TextBox();
			this->textValueDesc27 = new System::Windows::Forms::TextBox();
			this->textValueDesc26 = new System::Windows::Forms::TextBox();
			this->textValueDesc25 = new System::Windows::Forms::TextBox();
			this->textValueDesc24 = new System::Windows::Forms::TextBox();
			this->textValueDesc23 = new System::Windows::Forms::TextBox();
			this->textValueDesc22 = new System::Windows::Forms::TextBox();
			this->textValueDesc21 = new System::Windows::Forms::TextBox();
			this->textValueDesc20 = new System::Windows::Forms::TextBox();
			this->textValueDesc19 = new System::Windows::Forms::TextBox();
			this->textValueDesc18 = new System::Windows::Forms::TextBox();
			this->textValueDesc17 = new System::Windows::Forms::TextBox();
			this->textValueDesc16 = new System::Windows::Forms::TextBox();
			this->textValueDesc15 = new System::Windows::Forms::TextBox();
			this->textValueDesc14 = new System::Windows::Forms::TextBox();
			this->textValueDesc13 = new System::Windows::Forms::TextBox();
			this->textValueDesc12 = new System::Windows::Forms::TextBox();
			this->textValueDesc11 = new System::Windows::Forms::TextBox();
			this->textValueDesc10 = new System::Windows::Forms::TextBox();
			this->textValueDesc9 = new System::Windows::Forms::TextBox();
			this->textValueDesc8 = new System::Windows::Forms::TextBox();
			this->textValueDesc7 = new System::Windows::Forms::TextBox();
			this->textValueDesc6 = new System::Windows::Forms::TextBox();
			this->textValueDesc5 = new System::Windows::Forms::TextBox();
			this->textValueDesc4 = new System::Windows::Forms::TextBox();
			this->textValueDesc3 = new System::Windows::Forms::TextBox();
			this->textValueDesc2 = new System::Windows::Forms::TextBox();
			this->textValueDesc1 = new System::Windows::Forms::TextBox();
			this->textValueDesc0 = new System::Windows::Forms::TextBox();
			this->textValueName31 = new System::Windows::Forms::TextBox();
			this->textValueName30 = new System::Windows::Forms::TextBox();
			this->textValueName29 = new System::Windows::Forms::TextBox();
			this->textValueName28 = new System::Windows::Forms::TextBox();
			this->textValueName27 = new System::Windows::Forms::TextBox();
			this->textValueName26 = new System::Windows::Forms::TextBox();
			this->textValueName25 = new System::Windows::Forms::TextBox();
			this->textValueName24 = new System::Windows::Forms::TextBox();
			this->textValueName23 = new System::Windows::Forms::TextBox();
			this->textValueName22 = new System::Windows::Forms::TextBox();
			this->textValueName21 = new System::Windows::Forms::TextBox();
			this->textValueName20 = new System::Windows::Forms::TextBox();
			this->textValueName19 = new System::Windows::Forms::TextBox();
			this->textValueName18 = new System::Windows::Forms::TextBox();
			this->textValueName17 = new System::Windows::Forms::TextBox();
			this->textValueName16 = new System::Windows::Forms::TextBox();
			this->textValueName15 = new System::Windows::Forms::TextBox();
			this->textValueName14 = new System::Windows::Forms::TextBox();
			this->textValueName13 = new System::Windows::Forms::TextBox();
			this->textValueName12 = new System::Windows::Forms::TextBox();
			this->textValueName11 = new System::Windows::Forms::TextBox();
			this->textValueName10 = new System::Windows::Forms::TextBox();
			this->textValueName9 = new System::Windows::Forms::TextBox();
			this->textValueName8 = new System::Windows::Forms::TextBox();
			this->textValueName7 = new System::Windows::Forms::TextBox();
			this->textValueName6 = new System::Windows::Forms::TextBox();
			this->textValueName5 = new System::Windows::Forms::TextBox();
			this->textValueName4 = new System::Windows::Forms::TextBox();
			this->textValueName3 = new System::Windows::Forms::TextBox();
			this->textValueName2 = new System::Windows::Forms::TextBox();
			this->textValueName1 = new System::Windows::Forms::TextBox();
			this->textValueName0 = new System::Windows::Forms::TextBox();
			this->label5 = new System::Windows::Forms::Label();
			this->comboFeatureType = new System::Windows::Forms::ComboBox();
			this->label4 = new System::Windows::Forms::Label();
			this->textFeatureDescription = new System::Windows::Forms::TextBox();
			this->label3 = new System::Windows::Forms::Label();
			this->textFeatureName = new System::Windows::Forms::TextBox();
			this->labelPath = new System::Windows::Forms::Label();
			this->folderPath = new System::Windows::Forms::FolderBrowserDialog();
			this->toolTip1 = new System::Windows::Forms::ToolTip(this->components);
			this->btnExitNoSave = new System::Windows::Forms::Button();
			this->btnExitAndSave = new System::Windows::Forms::Button();
			this->pictureBox1 = new System::Windows::Forms::PictureBox();
			this->groupBox1 = new System::Windows::Forms::GroupBox();
			this->btnGenerate = new System::Windows::Forms::Button();
			this->groupFeatureSelection->SuspendLayout();
			this->groupFeatureEditor->SuspendLayout();
			this->groupIntegerFeature->SuspendLayout();
			this->groupMapFeature->SuspendLayout();
			this->groupBox1->SuspendLayout();
			this->SuspendLayout();
			// 
			// groupFeatureSelection
			// 
			this->groupFeatureSelection->Controls->Add(this->btnDeleteFeature);
			this->groupFeatureSelection->Controls->Add(this->btnAddFeature);
			this->groupFeatureSelection->Controls->Add(this->label2);
			this->groupFeatureSelection->Controls->Add(this->label1);
			this->groupFeatureSelection->Controls->Add(this->listFeatures);
			this->groupFeatureSelection->Controls->Add(this->comboFeatureGroup);
			this->groupFeatureSelection->Location = System::Drawing::Point(8, 24);
			this->groupFeatureSelection->Name = S"groupFeatureSelection";
			this->groupFeatureSelection->Size = System::Drawing::Size(304, 608);
			this->groupFeatureSelection->TabIndex = 1;
			this->groupFeatureSelection->TabStop = false;
			this->groupFeatureSelection->Text = S"Выбор характеристики для редактирования";
			// 
			// btnDeleteFeature
			// 
			//---this->btnDeleteFeature->Image = (__try_cast<System::Drawing::Image *  >(resources->GetObject(S"btnDeleteFeature.Image")));
			this->btnDeleteFeature->ImageAlign = System::Drawing::ContentAlignment::MiddleLeft;
			this->btnDeleteFeature->Location = System::Drawing::Point(160, 576);
			this->btnDeleteFeature->Name = S"btnDeleteFeature";
			this->btnDeleteFeature->Size = System::Drawing::Size(128, 24);
			this->btnDeleteFeature->TabIndex = 5;
			this->btnDeleteFeature->Text = S"Удалить";
			this->btnDeleteFeature->Click += new System::EventHandler(this, &FeatureFactoryGenerator::FeatureEditor::btnDeleteFeature_Click);
			// 
			// btnAddFeature
			// 
			//---this->btnAddFeature->Image = (__try_cast<System::Drawing::Image *  >(resources->GetObject(S"btnAddFeature.Image")));
			this->btnAddFeature->ImageAlign = System::Drawing::ContentAlignment::MiddleLeft;
			this->btnAddFeature->Location = System::Drawing::Point(16, 576);
			this->btnAddFeature->Name = S"btnAddFeature";
			this->btnAddFeature->Size = System::Drawing::Size(128, 24);
			this->btnAddFeature->TabIndex = 4;
			this->btnAddFeature->Text = S"Добавить";
			this->btnAddFeature->Click += new System::EventHandler(this, &FeatureFactoryGenerator::FeatureEditor::btnAddFeature_Click);
			// 
			// label2
			// 
			this->label2->Location = System::Drawing::Point(8, 24);
			this->label2->Name = S"label2";
			this->label2->Size = System::Drawing::Size(272, 16);
			this->label2->TabIndex = 3;
			this->label2->Text = S"Выберите группу характеристик:";
			// 
			// label1
			// 
			this->label1->Location = System::Drawing::Point(8, 88);
			this->label1->Name = S"label1";
			this->label1->Size = System::Drawing::Size(288, 16);
			this->label1->TabIndex = 2;
			this->label1->Text = S"Выберите характеристику для редактирования:";
			// 
			// listFeatures
			// 
			this->listFeatures->Font = new System::Drawing::Font(S"Courier New", 10, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, (System::Byte)204);
			this->listFeatures->ForeColor = System::Drawing::Color::Navy;
			this->listFeatures->ItemHeight = 16;
			this->listFeatures->Location = System::Drawing::Point(8, 112);
			this->listFeatures->Name = S"listFeatures";
			this->listFeatures->Size = System::Drawing::Size(288, 452);
			this->listFeatures->TabIndex = 1;
			this->listFeatures->SelectedIndexChanged += new System::EventHandler(this, &FeatureFactoryGenerator::FeatureEditor::listFeatures_SelectedIndexChanged);
			// 
			// comboFeatureGroup
			// 
			this->comboFeatureGroup->DropDownStyle = System::Windows::Forms::ComboBoxStyle::DropDownList;
			this->comboFeatureGroup->Location = System::Drawing::Point(8, 48);
			this->comboFeatureGroup->Name = S"comboFeatureGroup";
			this->comboFeatureGroup->Size = System::Drawing::Size(288, 21);
			this->comboFeatureGroup->TabIndex = 0;
			this->comboFeatureGroup->SelectedIndexChanged += new System::EventHandler(this, &FeatureFactoryGenerator::FeatureEditor::comboFeatureGroup_SelectedIndexChanged);
			// 
			// linkPath
			// 
			this->linkPath->Location = System::Drawing::Point(8, 5);
			this->linkPath->Name = S"linkPath";
			this->linkPath->Size = System::Drawing::Size(192, 16);
			this->linkPath->TabIndex = 2;
			this->linkPath->TabStop = true;
			this->linkPath->Text = S"Смотреть списки характеристик в :";
			this->linkPath->LinkClicked += new System::Windows::Forms::LinkLabelLinkClickedEventHandler(this, &FeatureFactoryGenerator::FeatureEditor::linkPath_LinkClicked);
			// 
			// groupFeatureEditor
			// 
			this->groupFeatureEditor->Controls->Add(this->btnUndoChanges);
			this->groupFeatureEditor->Controls->Add(this->btnSaveFeature);
			this->groupFeatureEditor->Controls->Add(this->groupIntegerFeature);
			this->groupFeatureEditor->Controls->Add(this->groupMapFeature);
			this->groupFeatureEditor->Controls->Add(this->label5);
			this->groupFeatureEditor->Controls->Add(this->comboFeatureType);
			this->groupFeatureEditor->Controls->Add(this->label4);
			this->groupFeatureEditor->Controls->Add(this->textFeatureDescription);
			this->groupFeatureEditor->Controls->Add(this->label3);
			this->groupFeatureEditor->Controls->Add(this->textFeatureName);
			this->groupFeatureEditor->Enabled = false;
			this->groupFeatureEditor->Location = System::Drawing::Point(320, 24);
			this->groupFeatureEditor->Name = S"groupFeatureEditor";
			this->groupFeatureEditor->Size = System::Drawing::Size(536, 712);
			this->groupFeatureEditor->TabIndex = 3;
			this->groupFeatureEditor->TabStop = false;
			this->groupFeatureEditor->Text = S"Окно редактирования характеристики";
			// 
			// btnUndoChanges
			// 
			//---this->btnUndoChanges->Image = (__try_cast<System::Drawing::Image *  >(resources->GetObject(S"btnUndoChanges.Image")));
			this->btnUndoChanges->ImageAlign = System::Drawing::ContentAlignment::MiddleLeft;
			this->btnUndoChanges->Location = System::Drawing::Point(216, 672);
			this->btnUndoChanges->Name = S"btnUndoChanges";
			this->btnUndoChanges->Size = System::Drawing::Size(168, 32);
			this->btnUndoChanges->TabIndex = 68;
			this->btnUndoChanges->Text = S"Отменить все изменения";
			this->btnUndoChanges->TextAlign = System::Drawing::ContentAlignment::MiddleRight;
			this->btnUndoChanges->Click += new System::EventHandler(this, &FeatureFactoryGenerator::FeatureEditor::btnUndoChanges_Click);
			// 
			// btnSaveFeature
			// 
			this->btnSaveFeature->Enabled = false;
			//---this->btnSaveFeature->Image = (__try_cast<System::Drawing::Image *  >(resources->GetObject(S"btnSaveFeature.Image")));
			this->btnSaveFeature->ImageAlign = System::Drawing::ContentAlignment::MiddleLeft;
			this->btnSaveFeature->Location = System::Drawing::Point(32, 672);
			this->btnSaveFeature->Name = S"btnSaveFeature";
			this->btnSaveFeature->Size = System::Drawing::Size(176, 32);
			this->btnSaveFeature->TabIndex = 67;
			this->btnSaveFeature->Text = S"Сохранить изменения";
			this->btnSaveFeature->Click += new System::EventHandler(this, &FeatureFactoryGenerator::FeatureEditor::btnSaveFeature_Click);
			// 
			// groupIntegerFeature
			// 
			this->groupIntegerFeature->Controls->Add(this->label6);
			this->groupIntegerFeature->Controls->Add(this->textIntegerDescription);
			this->groupIntegerFeature->Location = System::Drawing::Point(8, 112);
			this->groupIntegerFeature->Name = S"groupIntegerFeature";
			this->groupIntegerFeature->Size = System::Drawing::Size(520, 64);
			this->groupIntegerFeature->TabIndex = 7;
			this->groupIntegerFeature->TabStop = false;
			this->groupIntegerFeature->Text = S"Редактор характеристик, имеющих одно значение (integer)";
			// 
			// label6
			// 
			this->label6->Location = System::Drawing::Point(208, 24);
			this->label6->Name = S"label6";
			this->label6->Size = System::Drawing::Size(276, 34);
			this->label6->TabIndex = 5;
			this->label6->Text = S"Дополнительное описание типа (уточнение), например, \"точность\" для типа Percent";
			// 
			// textIntegerDescription
			// 
			this->textIntegerDescription->Location = System::Drawing::Point(12, 24);
			this->textIntegerDescription->Name = S"textIntegerDescription";
			this->textIntegerDescription->Size = System::Drawing::Size(184, 20);
			this->textIntegerDescription->TabIndex = 4;
			this->textIntegerDescription->Text = S"";
			this->textIntegerDescription->TextChanged += new System::EventHandler(this, &FeatureFactoryGenerator::FeatureEditor::textValueDesc16_TextChanged);
			// 
			// groupMapFeature
			// 
			this->groupMapFeature->Controls->Add(this->label36);
			this->groupMapFeature->Controls->Add(this->label37);
			this->groupMapFeature->Controls->Add(this->label38);
			this->groupMapFeature->Controls->Add(this->label31);
			this->groupMapFeature->Controls->Add(this->label32);
			this->groupMapFeature->Controls->Add(this->label33);
			this->groupMapFeature->Controls->Add(this->label34);
			this->groupMapFeature->Controls->Add(this->label35);
			this->groupMapFeature->Controls->Add(this->label26);
			this->groupMapFeature->Controls->Add(this->label27);
			this->groupMapFeature->Controls->Add(this->label28);
			this->groupMapFeature->Controls->Add(this->label29);
			this->groupMapFeature->Controls->Add(this->label30);
			this->groupMapFeature->Controls->Add(this->label18);
			this->groupMapFeature->Controls->Add(this->label19);
			this->groupMapFeature->Controls->Add(this->label20);
			this->groupMapFeature->Controls->Add(this->label21);
			this->groupMapFeature->Controls->Add(this->label22);
			this->groupMapFeature->Controls->Add(this->label23);
			this->groupMapFeature->Controls->Add(this->label24);
			this->groupMapFeature->Controls->Add(this->label25);
			this->groupMapFeature->Controls->Add(this->label14);
			this->groupMapFeature->Controls->Add(this->label15);
			this->groupMapFeature->Controls->Add(this->label16);
			this->groupMapFeature->Controls->Add(this->label17);
			this->groupMapFeature->Controls->Add(this->label13);
			this->groupMapFeature->Controls->Add(this->label12);
			this->groupMapFeature->Controls->Add(this->label11);
			this->groupMapFeature->Controls->Add(this->label10);
			this->groupMapFeature->Controls->Add(this->label9);
			this->groupMapFeature->Controls->Add(this->label8);
			this->groupMapFeature->Controls->Add(this->label7);
			this->groupMapFeature->Controls->Add(this->btnValue31);
			this->groupMapFeature->Controls->Add(this->checkValue31);
			this->groupMapFeature->Controls->Add(this->btnValue30);
			this->groupMapFeature->Controls->Add(this->checkValue30);
			this->groupMapFeature->Controls->Add(this->btnValue29);
			this->groupMapFeature->Controls->Add(this->checkValue29);
			this->groupMapFeature->Controls->Add(this->btnValue28);
			this->groupMapFeature->Controls->Add(this->checkValue28);
			this->groupMapFeature->Controls->Add(this->btnValue27);
			this->groupMapFeature->Controls->Add(this->checkValue27);
			this->groupMapFeature->Controls->Add(this->btnValue26);
			this->groupMapFeature->Controls->Add(this->checkValue26);
			this->groupMapFeature->Controls->Add(this->btnValue25);
			this->groupMapFeature->Controls->Add(this->checkValue25);
			this->groupMapFeature->Controls->Add(this->btnValue24);
			this->groupMapFeature->Controls->Add(this->checkValue24);
			this->groupMapFeature->Controls->Add(this->btnValue23);
			this->groupMapFeature->Controls->Add(this->checkValue23);
			this->groupMapFeature->Controls->Add(this->btnValue22);
			this->groupMapFeature->Controls->Add(this->checkValue22);
			this->groupMapFeature->Controls->Add(this->btnValue21);
			this->groupMapFeature->Controls->Add(this->checkValue21);
			this->groupMapFeature->Controls->Add(this->btnValue20);
			this->groupMapFeature->Controls->Add(this->checkValue20);
			this->groupMapFeature->Controls->Add(this->btnValue19);
			this->groupMapFeature->Controls->Add(this->checkValue19);
			this->groupMapFeature->Controls->Add(this->btnValue18);
			this->groupMapFeature->Controls->Add(this->checkValue18);
			this->groupMapFeature->Controls->Add(this->btnValue17);
			this->groupMapFeature->Controls->Add(this->checkValue17);
			this->groupMapFeature->Controls->Add(this->btnValue16);
			this->groupMapFeature->Controls->Add(this->checkValue16);
			this->groupMapFeature->Controls->Add(this->btnValue15);
			this->groupMapFeature->Controls->Add(this->checkValue15);
			this->groupMapFeature->Controls->Add(this->btnValue14);
			this->groupMapFeature->Controls->Add(this->checkValue14);
			this->groupMapFeature->Controls->Add(this->btnValue13);
			this->groupMapFeature->Controls->Add(this->checkValue13);
			this->groupMapFeature->Controls->Add(this->btnValue12);
			this->groupMapFeature->Controls->Add(this->checkValue12);
			this->groupMapFeature->Controls->Add(this->btnValue11);
			this->groupMapFeature->Controls->Add(this->checkValue11);
			this->groupMapFeature->Controls->Add(this->btnValue10);
			this->groupMapFeature->Controls->Add(this->checkValue10);
			this->groupMapFeature->Controls->Add(this->btnValue9);
			this->groupMapFeature->Controls->Add(this->checkValue9);
			this->groupMapFeature->Controls->Add(this->btnValue8);
			this->groupMapFeature->Controls->Add(this->checkValue8);
			this->groupMapFeature->Controls->Add(this->btnValue7);
			this->groupMapFeature->Controls->Add(this->checkValue7);
			this->groupMapFeature->Controls->Add(this->btnValue6);
			this->groupMapFeature->Controls->Add(this->checkValue6);
			this->groupMapFeature->Controls->Add(this->btnValue5);
			this->groupMapFeature->Controls->Add(this->checkValue5);
			this->groupMapFeature->Controls->Add(this->btnValue4);
			this->groupMapFeature->Controls->Add(this->checkValue4);
			this->groupMapFeature->Controls->Add(this->btnValue3);
			this->groupMapFeature->Controls->Add(this->checkValue3);
			this->groupMapFeature->Controls->Add(this->btnValue2);
			this->groupMapFeature->Controls->Add(this->checkValue2);
			this->groupMapFeature->Controls->Add(this->btnValue1);
			this->groupMapFeature->Controls->Add(this->checkValue1);
			this->groupMapFeature->Controls->Add(this->btnValue0);
			this->groupMapFeature->Controls->Add(this->checkValue0);
			this->groupMapFeature->Controls->Add(this->textValueDesc31);
			this->groupMapFeature->Controls->Add(this->textValueDesc30);
			this->groupMapFeature->Controls->Add(this->textValueDesc29);
			this->groupMapFeature->Controls->Add(this->textValueDesc28);
			this->groupMapFeature->Controls->Add(this->textValueDesc27);
			this->groupMapFeature->Controls->Add(this->textValueDesc26);
			this->groupMapFeature->Controls->Add(this->textValueDesc25);
			this->groupMapFeature->Controls->Add(this->textValueDesc24);
			this->groupMapFeature->Controls->Add(this->textValueDesc23);
			this->groupMapFeature->Controls->Add(this->textValueDesc22);
			this->groupMapFeature->Controls->Add(this->textValueDesc21);
			this->groupMapFeature->Controls->Add(this->textValueDesc20);
			this->groupMapFeature->Controls->Add(this->textValueDesc19);
			this->groupMapFeature->Controls->Add(this->textValueDesc18);
			this->groupMapFeature->Controls->Add(this->textValueDesc17);
			this->groupMapFeature->Controls->Add(this->textValueDesc16);
			this->groupMapFeature->Controls->Add(this->textValueDesc15);
			this->groupMapFeature->Controls->Add(this->textValueDesc14);
			this->groupMapFeature->Controls->Add(this->textValueDesc13);
			this->groupMapFeature->Controls->Add(this->textValueDesc12);
			this->groupMapFeature->Controls->Add(this->textValueDesc11);
			this->groupMapFeature->Controls->Add(this->textValueDesc10);
			this->groupMapFeature->Controls->Add(this->textValueDesc9);
			this->groupMapFeature->Controls->Add(this->textValueDesc8);
			this->groupMapFeature->Controls->Add(this->textValueDesc7);
			this->groupMapFeature->Controls->Add(this->textValueDesc6);
			this->groupMapFeature->Controls->Add(this->textValueDesc5);
			this->groupMapFeature->Controls->Add(this->textValueDesc4);
			this->groupMapFeature->Controls->Add(this->textValueDesc3);
			this->groupMapFeature->Controls->Add(this->textValueDesc2);
			this->groupMapFeature->Controls->Add(this->textValueDesc1);
			this->groupMapFeature->Controls->Add(this->textValueDesc0);
			this->groupMapFeature->Controls->Add(this->textValueName31);
			this->groupMapFeature->Controls->Add(this->textValueName30);
			this->groupMapFeature->Controls->Add(this->textValueName29);
			this->groupMapFeature->Controls->Add(this->textValueName28);
			this->groupMapFeature->Controls->Add(this->textValueName27);
			this->groupMapFeature->Controls->Add(this->textValueName26);
			this->groupMapFeature->Controls->Add(this->textValueName25);
			this->groupMapFeature->Controls->Add(this->textValueName24);
			this->groupMapFeature->Controls->Add(this->textValueName23);
			this->groupMapFeature->Controls->Add(this->textValueName22);
			this->groupMapFeature->Controls->Add(this->textValueName21);
			this->groupMapFeature->Controls->Add(this->textValueName20);
			this->groupMapFeature->Controls->Add(this->textValueName19);
			this->groupMapFeature->Controls->Add(this->textValueName18);
			this->groupMapFeature->Controls->Add(this->textValueName17);
			this->groupMapFeature->Controls->Add(this->textValueName16);
			this->groupMapFeature->Controls->Add(this->textValueName15);
			this->groupMapFeature->Controls->Add(this->textValueName14);
			this->groupMapFeature->Controls->Add(this->textValueName13);
			this->groupMapFeature->Controls->Add(this->textValueName12);
			this->groupMapFeature->Controls->Add(this->textValueName11);
			this->groupMapFeature->Controls->Add(this->textValueName10);
			this->groupMapFeature->Controls->Add(this->textValueName9);
			this->groupMapFeature->Controls->Add(this->textValueName8);
			this->groupMapFeature->Controls->Add(this->textValueName7);
			this->groupMapFeature->Controls->Add(this->textValueName6);
			this->groupMapFeature->Controls->Add(this->textValueName5);
			this->groupMapFeature->Controls->Add(this->textValueName4);
			this->groupMapFeature->Controls->Add(this->textValueName3);
			this->groupMapFeature->Controls->Add(this->textValueName2);
			this->groupMapFeature->Controls->Add(this->textValueName1);
			this->groupMapFeature->Controls->Add(this->textValueName0);
			this->groupMapFeature->Location = System::Drawing::Point(8, 184);
			this->groupMapFeature->Name = S"groupMapFeature";
			this->groupMapFeature->Size = System::Drawing::Size(520, 480);
			this->groupMapFeature->TabIndex = 6;
			this->groupMapFeature->TabStop = false;
			this->groupMapFeature->Text = S"Редактор \"Мап\" характеристик";
			// 
			// label36
			// 
			this->label36->Location = System::Drawing::Point(6, 444);
			this->label36->Name = S"label36";
			this->label36->Size = System::Drawing::Size(18, 16);
			this->label36->TabIndex = 159;
			this->label36->Text = S"30";
			this->label36->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label37
			// 
			this->label37->Location = System::Drawing::Point(6, 430);
			this->label37->Name = S"label37";
			this->label37->Size = System::Drawing::Size(18, 16);
			this->label37->TabIndex = 158;
			this->label37->Text = S"29";
			this->label37->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label38
			// 
			this->label38->Location = System::Drawing::Point(6, 416);
			this->label38->Name = S"label38";
			this->label38->Size = System::Drawing::Size(18, 16);
			this->label38->TabIndex = 157;
			this->label38->Text = S"28";
			this->label38->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label31
			// 
			this->label31->Location = System::Drawing::Point(6, 402);
			this->label31->Name = S"label31";
			this->label31->Size = System::Drawing::Size(18, 16);
			this->label31->TabIndex = 156;
			this->label31->Text = S"27";
			this->label31->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label32
			// 
			this->label32->Location = System::Drawing::Point(6, 388);
			this->label32->Name = S"label32";
			this->label32->Size = System::Drawing::Size(18, 16);
			this->label32->TabIndex = 155;
			this->label32->Text = S"26";
			this->label32->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label33
			// 
			this->label33->Location = System::Drawing::Point(6, 374);
			this->label33->Name = S"label33";
			this->label33->Size = System::Drawing::Size(18, 16);
			this->label33->TabIndex = 154;
			this->label33->Text = S"25";
			this->label33->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label34
			// 
			this->label34->Location = System::Drawing::Point(6, 360);
			this->label34->Name = S"label34";
			this->label34->Size = System::Drawing::Size(18, 16);
			this->label34->TabIndex = 153;
			this->label34->Text = S"24";
			this->label34->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label35
			// 
			this->label35->Location = System::Drawing::Point(6, 346);
			this->label35->Name = S"label35";
			this->label35->Size = System::Drawing::Size(18, 16);
			this->label35->TabIndex = 152;
			this->label35->Text = S"23";
			this->label35->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label26
			// 
			this->label26->Location = System::Drawing::Point(6, 318);
			this->label26->Name = S"label26";
			this->label26->Size = System::Drawing::Size(18, 16);
			this->label26->TabIndex = 151;
			this->label26->Text = S"21";
			this->label26->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label27
			// 
			this->label27->Location = System::Drawing::Point(6, 304);
			this->label27->Name = S"label27";
			this->label27->Size = System::Drawing::Size(18, 16);
			this->label27->TabIndex = 150;
			this->label27->Text = S"20";
			this->label27->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label28
			// 
			this->label28->Location = System::Drawing::Point(6, 290);
			this->label28->Name = S"label28";
			this->label28->Size = System::Drawing::Size(18, 16);
			this->label28->TabIndex = 149;
			this->label28->Text = S"19";
			this->label28->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label29
			// 
			this->label29->Location = System::Drawing::Point(6, 276);
			this->label29->Name = S"label29";
			this->label29->Size = System::Drawing::Size(18, 16);
			this->label29->TabIndex = 148;
			this->label29->Text = S"18";
			this->label29->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label30
			// 
			this->label30->Location = System::Drawing::Point(6, 262);
			this->label30->Name = S"label30";
			this->label30->Size = System::Drawing::Size(18, 16);
			this->label30->TabIndex = 147;
			this->label30->Text = S"17";
			this->label30->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label18
			// 
			this->label18->Location = System::Drawing::Point(6, 248);
			this->label18->Name = S"label18";
			this->label18->Size = System::Drawing::Size(18, 16);
			this->label18->TabIndex = 146;
			this->label18->Text = S"16";
			this->label18->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label19
			// 
			this->label19->Location = System::Drawing::Point(6, 234);
			this->label19->Name = S"label19";
			this->label19->Size = System::Drawing::Size(18, 16);
			this->label19->TabIndex = 145;
			this->label19->Text = S"15";
			this->label19->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label20
			// 
			this->label20->Location = System::Drawing::Point(6, 220);
			this->label20->Name = S"label20";
			this->label20->Size = System::Drawing::Size(18, 16);
			this->label20->TabIndex = 144;
			this->label20->Text = S"14";
			this->label20->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label21
			// 
			this->label21->Location = System::Drawing::Point(6, 206);
			this->label21->Name = S"label21";
			this->label21->Size = System::Drawing::Size(18, 16);
			this->label21->TabIndex = 143;
			this->label21->Text = S"13";
			this->label21->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label22
			// 
			this->label22->Location = System::Drawing::Point(6, 192);
			this->label22->Name = S"label22";
			this->label22->Size = System::Drawing::Size(18, 16);
			this->label22->TabIndex = 142;
			this->label22->Text = S"12";
			this->label22->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label23
			// 
			this->label23->Location = System::Drawing::Point(6, 178);
			this->label23->Name = S"label23";
			this->label23->Size = System::Drawing::Size(18, 16);
			this->label23->TabIndex = 141;
			this->label23->Text = S"11";
			this->label23->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label24
			// 
			this->label24->Location = System::Drawing::Point(6, 164);
			this->label24->Name = S"label24";
			this->label24->Size = System::Drawing::Size(18, 16);
			this->label24->TabIndex = 140;
			this->label24->Text = S"10";
			this->label24->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label25
			// 
			this->label25->Location = System::Drawing::Point(6, 150);
			this->label25->Name = S"label25";
			this->label25->Size = System::Drawing::Size(18, 16);
			this->label25->TabIndex = 139;
			this->label25->Text = S"9";
			this->label25->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label14
			// 
			this->label14->Location = System::Drawing::Point(6, 136);
			this->label14->Name = S"label14";
			this->label14->Size = System::Drawing::Size(18, 16);
			this->label14->TabIndex = 138;
			this->label14->Text = S"8";
			this->label14->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label15
			// 
			this->label15->Location = System::Drawing::Point(6, 122);
			this->label15->Name = S"label15";
			this->label15->Size = System::Drawing::Size(18, 16);
			this->label15->TabIndex = 137;
			this->label15->Text = S"7";
			this->label15->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label16
			// 
			this->label16->Location = System::Drawing::Point(6, 108);
			this->label16->Name = S"label16";
			this->label16->Size = System::Drawing::Size(18, 16);
			this->label16->TabIndex = 136;
			this->label16->Text = S"6";
			this->label16->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label17
			// 
			this->label17->Location = System::Drawing::Point(6, 94);
			this->label17->Name = S"label17";
			this->label17->Size = System::Drawing::Size(18, 16);
			this->label17->TabIndex = 135;
			this->label17->Text = S"5";
			this->label17->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label13
			// 
			this->label13->Location = System::Drawing::Point(6, 80);
			this->label13->Name = S"label13";
			this->label13->Size = System::Drawing::Size(18, 16);
			this->label13->TabIndex = 134;
			this->label13->Text = S"4";
			this->label13->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label12
			// 
			this->label12->Location = System::Drawing::Point(6, 66);
			this->label12->Name = S"label12";
			this->label12->Size = System::Drawing::Size(18, 16);
			this->label12->TabIndex = 133;
			this->label12->Text = S"3";
			this->label12->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label11
			// 
			this->label11->Location = System::Drawing::Point(6, 52);
			this->label11->Name = S"label11";
			this->label11->Size = System::Drawing::Size(18, 16);
			this->label11->TabIndex = 132;
			this->label11->Text = S"2";
			this->label11->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label10
			// 
			this->label10->Location = System::Drawing::Point(6, 38);
			this->label10->Name = S"label10";
			this->label10->Size = System::Drawing::Size(18, 16);
			this->label10->TabIndex = 131;
			this->label10->Text = S"1";
			this->label10->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label9
			// 
			this->label9->Location = System::Drawing::Point(6, 332);
			this->label9->Name = S"label9";
			this->label9->Size = System::Drawing::Size(18, 16);
			this->label9->TabIndex = 130;
			this->label9->Text = S"22";
			this->label9->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label8
			// 
			this->label8->Location = System::Drawing::Point(6, 457);
			this->label8->Name = S"label8";
			this->label8->Size = System::Drawing::Size(18, 16);
			this->label8->TabIndex = 129;
			this->label8->Text = S"31";
			this->label8->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label7
			// 
			this->label7->Location = System::Drawing::Point(6, 24);
			this->label7->Name = S"label7";
			this->label7->Size = System::Drawing::Size(18, 16);
			this->label7->TabIndex = 128;
			this->label7->Text = S"0";
			this->label7->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// btnValue31
			// 
			this->btnValue31->ImageIndex = 0;
			this->btnValue31->ImageList = this->imageList1;
			this->btnValue31->Location = System::Drawing::Point(384, 458);
			this->btnValue31->Name = S"btnValue31";
			this->btnValue31->Size = System::Drawing::Size(16, 14);
			this->btnValue31->TabIndex = 127;
			this->btnValue31->Tag = S"31";
			this->toolTip1->SetToolTip(this->btnValue31, S"Удалить");
			this->btnValue31->Click += new System::EventHandler(this, &FeatureFactoryGenerator::FeatureEditor::btnValue28_Click);
			// 
			// imageList1
			// 
			this->imageList1->ImageSize = System::Drawing::Size(12, 12);
			//---this->imageList1->ImageStream = (__try_cast<System::Windows::Forms::ImageListStreamer *  >(resources->GetObject(S"imageList1.ImageStream")));
			this->imageList1->TransparentColor = System::Drawing::Color::Transparent;
			// 
			// checkValue31
			// 
			this->checkValue31->Enabled = false;
			this->checkValue31->Location = System::Drawing::Point(408, 458);
			this->checkValue31->Name = S"checkValue31";
			this->checkValue31->Size = System::Drawing::Size(110, 16);
			this->checkValue31->TabIndex = 126;
			this->checkValue31->Text = S"не используется";
			this->toolTip1->SetToolTip(this->checkValue31, S"используется/не используется");
			this->checkValue31->CheckedChanged += new System::EventHandler(this, &FeatureFactoryGenerator::FeatureEditor::checkValue16_CheckedChanged);
			// 
			// btnValue30
			// 
			this->btnValue30->ImageIndex = 0;
			this->btnValue30->ImageList = this->imageList1;
			this->btnValue30->Location = System::Drawing::Point(384, 444);
			this->btnValue30->Name = S"btnValue30";
			this->btnValue30->Size = System::Drawing::Size(16, 14);
			this->btnValue30->TabIndex = 125;
			this->btnValue30->Tag = S"30";
			this->toolTip1->SetToolTip(this->btnValue30, S"Удалить");
			this->btnValue30->Click += new System::EventHandler(this, &FeatureFactoryGenerator::FeatureEditor::btnValue28_Click);
			// 
			// checkValue30
			// 
			this->checkValue30->Enabled = false;
			this->checkValue30->Location = System::Drawing::Point(408, 444);
			this->checkValue30->Name = S"checkValue30";
			this->checkValue30->Size = System::Drawing::Size(110, 16);
			this->checkValue30->TabIndex = 124;
			this->checkValue30->Text = S"не используется";
			this->toolTip1->SetToolTip(this->checkValue30, S"используется/не используется");
			this->checkValue30->CheckedChanged += new System::EventHandler(this, &FeatureFactoryGenerator::FeatureEditor::checkValue16_CheckedChanged);
			// 
			// btnValue29
			// 
			this->btnValue29->ImageIndex = 0;
			this->btnValue29->ImageList = this->imageList1;
			this->btnValue29->Location = System::Drawing::Point(384, 430);
			this->btnValue29->Name = S"btnValue29";
			this->btnValue29->Size = System::Drawing::Size(16, 14);
			this->btnValue29->TabIndex = 123;
			this->btnValue29->Tag = S"29";
			this->toolTip1->SetToolTip(this->btnValue29, S"Удалить");
			this->btnValue29->Click += new System::EventHandler(this, &FeatureFactoryGenerator::FeatureEditor::btnValue28_Click);
			// 
			// checkValue29
			// 
			this->checkValue29->Enabled = false;
			this->checkValue29->Location = System::Drawing::Point(408, 430);
			this->checkValue29->Name = S"checkValue29";
			this->checkValue29->Size = System::Drawing::Size(110, 16);
			this->checkValue29->TabIndex = 122;
			this->checkValue29->Text = S"не используется";
			this->toolTip1->SetToolTip(this->checkValue29, S"используется/не используется");
			this->checkValue29->CheckedChanged += new System::EventHandler(this, &FeatureFactoryGenerator::FeatureEditor::checkValue16_CheckedChanged);
			// 
			// btnValue28
			// 
			this->btnValue28->ImageIndex = 0;
			this->btnValue28->ImageList = this->imageList1;
			this->btnValue28->Location = System::Drawing::Point(384, 416);
			this->btnValue28->Name = S"btnValue28";
			this->btnValue28->Size = System::Drawing::Size(16, 14);
			this->btnValue28->TabIndex = 121;
			this->btnValue28->Tag = S"28";
			this->toolTip1->SetToolTip(this->btnValue28, S"Удалить");
			this->btnValue28->Click += new System::EventHandler(this, &FeatureFactoryGenerator::FeatureEditor::btnValue28_Click);
			// 
			// checkValue28
			// 
			this->checkValue28->Enabled = false;
			this->checkValue28->Location = System::Drawing::Point(408, 416);
			this->checkValue28->Name = S"checkValue28";
			this->checkValue28->Size = System::Drawing::Size(110, 16);
			this->checkValue28->TabIndex = 120;
			this->checkValue28->Text = S"не используется";
			this->toolTip1->SetToolTip(this->checkValue28, S"используется/не используется");
			this->checkValue28->CheckedChanged += new System::EventHandler(this, &FeatureFactoryGenerator::FeatureEditor::checkValue16_CheckedChanged);
			// 
			// btnValue27
			// 
			this->btnValue27->ImageIndex = 0;
			this->btnValue27->ImageList = this->imageList1;
			this->btnValue27->Location = System::Drawing::Point(384, 402);
			this->btnValue27->Name = S"btnValue27";
			this->btnValue27->Size = System::Drawing::Size(16, 14);
			this->btnValue27->TabIndex = 119;
			this->btnValue27->Tag = S"27";
			this->toolTip1->SetToolTip(this->btnValue27, S"Удалить");
			this->btnValue27->Click += new System::EventHandler(this, &FeatureFactoryGenerator::FeatureEditor::btnValue28_Click);
			// 
			// checkValue27
			// 
			this->checkValue27->Enabled = false;
			this->checkValue27->Location = System::Drawing::Point(408, 402);
			this->checkValue27->Name = S"checkValue27";
			this->checkValue27->Size = System::Drawing::Size(110, 16);
			this->checkValue27->TabIndex = 118;
			this->checkValue27->Text = S"не используется";
			this->toolTip1->SetToolTip(this->checkValue27, S"используется/не используется");
			this->checkValue27->CheckedChanged += new System::EventHandler(this, &FeatureFactoryGenerator::FeatureEditor::checkValue16_CheckedChanged);
			// 
			// btnValue26
			// 
			this->btnValue26->ImageIndex = 0;
			this->btnValue26->ImageList = this->imageList1;
			this->btnValue26->Location = System::Drawing::Point(384, 388);
			this->btnValue26->Name = S"btnValue26";
			this->btnValue26->Size = System::Drawing::Size(16, 14);
			this->btnValue26->TabIndex = 117;
			this->btnValue26->Tag = S"26";
			this->toolTip1->SetToolTip(this->btnValue26, S"Удалить");
			this->btnValue26->Click += new System::EventHandler(this, &FeatureFactoryGenerator::FeatureEditor::btnValue28_Click);
			// 
			// checkValue26
			// 
			this->checkValue26->Enabled = false;
			this->checkValue26->Location = System::Drawing::Point(408, 388);
			this->checkValue26->Name = S"checkValue26";
			this->checkValue26->Size = System::Drawing::Size(110, 16);
			this->checkValue26->TabIndex = 116;
			this->checkValue26->Text = S"не используется";
			this->toolTip1->SetToolTip(this->checkValue26, S"используется/не используется");
			this->checkValue26->CheckedChanged += new System::EventHandler(this, &FeatureFactoryGenerator::FeatureEditor::checkValue16_CheckedChanged);
			// 
			// btnValue25
			// 
			this->btnValue25->ImageIndex = 0;
			this->btnValue25->ImageList = this->imageList1;
			this->btnValue25->Location = System::Drawing::Point(384, 374);
			this->btnValue25->Name = S"btnValue25";
			this->btnValue25->Size = System::Drawing::Size(16, 14);
			this->btnValue25->TabIndex = 115;
			this->btnValue25->Tag = S"25";
			this->toolTip1->SetToolTip(this->btnValue25, S"Удалить");
			this->btnValue25->Click += new System::EventHandler(this, &FeatureFactoryGenerator::FeatureEditor::btnValue28_Click);
			// 
			// checkValue25
			// 
			this->checkValue25->Enabled = false;
			this->checkValue25->Location = System::Drawing::Point(408, 374);
			this->checkValue25->Name = S"checkValue25";
			this->checkValue25->Size = System::Drawing::Size(110, 16);
			this->checkValue25->TabIndex = 114;
			this->checkValue25->Text = S"не используется";
			this->toolTip1->SetToolTip(this->checkValue25, S"используется/не используется");
			this->checkValue25->CheckedChanged += new System::EventHandler(this, &FeatureFactoryGenerator::FeatureEditor::checkValue16_CheckedChanged);
			// 
			// btnValue24
			// 
			this->btnValue24->ImageIndex = 0;
			this->btnValue24->ImageList = this->imageList1;
			this->btnValue24->Location = System::Drawing::Point(384, 360);
			this->btnValue24->Name = S"btnValue24";
			this->btnValue24->Size = System::Drawing::Size(16, 14);
			this->btnValue24->TabIndex = 113;
			this->btnValue24->Tag = S"24";
			this->toolTip1->SetToolTip(this->btnValue24, S"Удалить");
			this->btnValue24->Click += new System::EventHandler(this, &FeatureFactoryGenerator::FeatureEditor::btnValue28_Click);
			// 
			// checkValue24
			// 
			this->checkValue24->Enabled = false;
			this->checkValue24->Location = System::Drawing::Point(408, 360);
			this->checkValue24->Name = S"checkValue24";
			this->checkValue24->Size = System::Drawing::Size(110, 16);
			this->checkValue24->TabIndex = 112;
			this->checkValue24->Text = S"не используется";
			this->toolTip1->SetToolTip(this->checkValue24, S"используется/не используется");
			this->checkValue24->CheckedChanged += new System::EventHandler(this, &FeatureFactoryGenerator::FeatureEditor::checkValue16_CheckedChanged);
			// 
			// btnValue23
			// 
			this->btnValue23->ImageIndex = 0;
			this->btnValue23->ImageList = this->imageList1;
			this->btnValue23->Location = System::Drawing::Point(384, 346);
			this->btnValue23->Name = S"btnValue23";
			this->btnValue23->Size = System::Drawing::Size(16, 14);
			this->btnValue23->TabIndex = 111;
			this->btnValue23->Tag = S"23";
			this->toolTip1->SetToolTip(this->btnValue23, S"Удалить");
			this->btnValue23->Click += new System::EventHandler(this, &FeatureFactoryGenerator::FeatureEditor::btnValue28_Click);
			// 
			// checkValue23
			// 
			this->checkValue23->Enabled = false;
			this->checkValue23->Location = System::Drawing::Point(408, 346);
			this->checkValue23->Name = S"checkValue23";
			this->checkValue23->Size = System::Drawing::Size(110, 16);
			this->checkValue23->TabIndex = 110;
			this->checkValue23->Text = S"не используется";
			this->toolTip1->SetToolTip(this->checkValue23, S"используется/не используется");
			this->checkValue23->CheckedChanged += new System::EventHandler(this, &FeatureFactoryGenerator::FeatureEditor::checkValue16_CheckedChanged);
			// 
			// btnValue22
			// 
			this->btnValue22->ImageIndex = 0;
			this->btnValue22->ImageList = this->imageList1;
			this->btnValue22->Location = System::Drawing::Point(384, 332);
			this->btnValue22->Name = S"btnValue22";
			this->btnValue22->Size = System::Drawing::Size(16, 14);
			this->btnValue22->TabIndex = 109;
			this->btnValue22->Tag = S"22";
			this->toolTip1->SetToolTip(this->btnValue22, S"Удалить");
			this->btnValue22->Click += new System::EventHandler(this, &FeatureFactoryGenerator::FeatureEditor::btnValue28_Click);
			// 
			// checkValue22
			// 
			this->checkValue22->Enabled = false;
			this->checkValue22->Location = System::Drawing::Point(408, 332);
			this->checkValue22->Name = S"checkValue22";
			this->checkValue22->Size = System::Drawing::Size(110, 16);
			this->checkValue22->TabIndex = 108;
			this->checkValue22->Text = S"не используется";
			this->toolTip1->SetToolTip(this->checkValue22, S"используется/не используется");
			this->checkValue22->CheckedChanged += new System::EventHandler(this, &FeatureFactoryGenerator::FeatureEditor::checkValue16_CheckedChanged);
			// 
			// btnValue21
			// 
			this->btnValue21->ImageIndex = 0;
			this->btnValue21->ImageList = this->imageList1;
			this->btnValue21->Location = System::Drawing::Point(384, 318);
			this->btnValue21->Name = S"btnValue21";
			this->btnValue21->Size = System::Drawing::Size(16, 14);
			this->btnValue21->TabIndex = 107;
			this->btnValue21->Tag = S"21";
			this->toolTip1->SetToolTip(this->btnValue21, S"Удалить");
			this->btnValue21->Click += new System::EventHandler(this, &FeatureFactoryGenerator::FeatureEditor::btnValue28_Click);
			// 
			// checkValue21
			// 
			this->checkValue21->Enabled = false;
			this->checkValue21->Location = System::Drawing::Point(408, 318);
			this->checkValue21->Name = S"checkValue21";
			this->checkValue21->Size = System::Drawing::Size(110, 16);
			this->checkValue21->TabIndex = 106;
			this->checkValue21->Text = S"не используется";
			this->toolTip1->SetToolTip(this->checkValue21, S"используется/не используется");
			this->checkValue21->CheckedChanged += new System::EventHandler(this, &FeatureFactoryGenerator::FeatureEditor::checkValue16_CheckedChanged);
			// 
			// btnValue20
			// 
			this->btnValue20->ImageIndex = 0;
			this->btnValue20->ImageList = this->imageList1;
			this->btnValue20->Location = System::Drawing::Point(384, 304);
			this->btnValue20->Name = S"btnValue20";
			this->btnValue20->Size = System::Drawing::Size(16, 14);
			this->btnValue20->TabIndex = 105;
			this->btnValue20->Tag = S"20";
			this->toolTip1->SetToolTip(this->btnValue20, S"Удалить");
			this->btnValue20->Click += new System::EventHandler(this, &FeatureFactoryGenerator::FeatureEditor::btnValue28_Click);
			// 
			// checkValue20
			// 
			this->checkValue20->Enabled = false;
			this->checkValue20->Location = System::Drawing::Point(408, 304);
			this->checkValue20->Name = S"checkValue20";
			this->checkValue20->Size = System::Drawing::Size(110, 16);
			this->checkValue20->TabIndex = 104;
			this->checkValue20->Text = S"не используется";
			this->toolTip1->SetToolTip(this->checkValue20, S"используется/не используется");
			this->checkValue20->CheckedChanged += new System::EventHandler(this, &FeatureFactoryGenerator::FeatureEditor::checkValue16_CheckedChanged);
			// 
			// btnValue19
			// 
			this->btnValue19->ImageIndex = 0;
			this->btnValue19->ImageList = this->imageList1;
			this->btnValue19->Location = System::Drawing::Point(384, 290);
			this->btnValue19->Name = S"btnValue19";
			this->btnValue19->Size = System::Drawing::Size(16, 14);
			this->btnValue19->TabIndex = 103;
			this->btnValue19->Tag = S"19";
			this->toolTip1->SetToolTip(this->btnValue19, S"Удалить");
			this->btnValue19->Click += new System::EventHandler(this, &FeatureFactoryGenerator::FeatureEditor::btnValue28_Click);
			// 
			// checkValue19
			// 
			this->checkValue19->Enabled = false;
			this->checkValue19->Location = System::Drawing::Point(408, 290);
			this->checkValue19->Name = S"checkValue19";
			this->checkValue19->Size = System::Drawing::Size(110, 16);
			this->checkValue19->TabIndex = 102;
			this->checkValue19->Text = S"не используется";
			this->toolTip1->SetToolTip(this->checkValue19, S"используется/не используется");
			this->checkValue19->CheckedChanged += new System::EventHandler(this, &FeatureFactoryGenerator::FeatureEditor::checkValue16_CheckedChanged);
			// 
			// btnValue18
			// 
			this->btnValue18->ImageIndex = 0;
			this->btnValue18->ImageList = this->imageList1;
			this->btnValue18->Location = System::Drawing::Point(384, 276);
			this->btnValue18->Name = S"btnValue18";
			this->btnValue18->Size = System::Drawing::Size(16, 14);
			this->btnValue18->TabIndex = 101;
			this->btnValue18->Tag = S"18";
			this->toolTip1->SetToolTip(this->btnValue18, S"Удалить");
			this->btnValue18->Click += new System::EventHandler(this, &FeatureFactoryGenerator::FeatureEditor::btnValue28_Click);
			// 
			// checkValue18
			// 
			this->checkValue18->Enabled = false;
			this->checkValue18->Location = System::Drawing::Point(408, 276);
			this->checkValue18->Name = S"checkValue18";
			this->checkValue18->Size = System::Drawing::Size(110, 16);
			this->checkValue18->TabIndex = 100;
			this->checkValue18->Text = S"не используется";
			this->toolTip1->SetToolTip(this->checkValue18, S"используется/не используется");
			this->checkValue18->CheckedChanged += new System::EventHandler(this, &FeatureFactoryGenerator::FeatureEditor::checkValue16_CheckedChanged);
			// 
			// btnValue17
			// 
			this->btnValue17->ImageIndex = 0;
			this->btnValue17->ImageList = this->imageList1;
			this->btnValue17->Location = System::Drawing::Point(384, 262);
			this->btnValue17->Name = S"btnValue17";
			this->btnValue17->Size = System::Drawing::Size(16, 14);
			this->btnValue17->TabIndex = 99;
			this->btnValue17->Tag = S"17";
			this->toolTip1->SetToolTip(this->btnValue17, S"Удалить");
			this->btnValue17->Click += new System::EventHandler(this, &FeatureFactoryGenerator::FeatureEditor::btnValue28_Click);
			// 
			// checkValue17
			// 
			this->checkValue17->Enabled = false;
			this->checkValue17->Location = System::Drawing::Point(408, 262);
			this->checkValue17->Name = S"checkValue17";
			this->checkValue17->Size = System::Drawing::Size(110, 16);
			this->checkValue17->TabIndex = 98;
			this->checkValue17->Text = S"не используется";
			this->toolTip1->SetToolTip(this->checkValue17, S"используется/не используется");
			this->checkValue17->CheckedChanged += new System::EventHandler(this, &FeatureFactoryGenerator::FeatureEditor::checkValue16_CheckedChanged);
			// 
			// btnValue16
			// 
			this->btnValue16->ImageIndex = 0;
			this->btnValue16->ImageList = this->imageList1;
			this->btnValue16->Location = System::Drawing::Point(384, 248);
			this->btnValue16->Name = S"btnValue16";
			this->btnValue16->Size = System::Drawing::Size(16, 14);
			this->btnValue16->TabIndex = 97;
			this->btnValue16->Tag = S"16";
			this->toolTip1->SetToolTip(this->btnValue16, S"Удалить");
			this->btnValue16->Click += new System::EventHandler(this, &FeatureFactoryGenerator::FeatureEditor::btnValue28_Click);
			// 
			// checkValue16
			// 
			this->checkValue16->Enabled = false;
			this->checkValue16->Location = System::Drawing::Point(408, 248);
			this->checkValue16->Name = S"checkValue16";
			this->checkValue16->Size = System::Drawing::Size(110, 16);
			this->checkValue16->TabIndex = 96;
			this->checkValue16->Text = S"не используется";
			this->toolTip1->SetToolTip(this->checkValue16, S"используется/не используется");
			this->checkValue16->CheckedChanged += new System::EventHandler(this, &FeatureFactoryGenerator::FeatureEditor::checkValue16_CheckedChanged);
			// 
			// btnValue15
			// 
			this->btnValue15->ImageIndex = 0;
			this->btnValue15->ImageList = this->imageList1;
			this->btnValue15->Location = System::Drawing::Point(384, 234);
			this->btnValue15->Name = S"btnValue15";
			this->btnValue15->Size = System::Drawing::Size(16, 14);
			this->btnValue15->TabIndex = 95;
			this->btnValue15->Tag = S"15";
			this->toolTip1->SetToolTip(this->btnValue15, S"Удалить");
			this->btnValue15->Click += new System::EventHandler(this, &FeatureFactoryGenerator::FeatureEditor::btnValue28_Click);
			// 
			// checkValue15
			// 
			this->checkValue15->Enabled = false;
			this->checkValue15->Location = System::Drawing::Point(408, 234);
			this->checkValue15->Name = S"checkValue15";
			this->checkValue15->Size = System::Drawing::Size(110, 16);
			this->checkValue15->TabIndex = 94;
			this->checkValue15->Text = S"не используется";
			this->toolTip1->SetToolTip(this->checkValue15, S"используется/не используется");
			this->checkValue15->CheckedChanged += new System::EventHandler(this, &FeatureFactoryGenerator::FeatureEditor::checkValue16_CheckedChanged);
			// 
			// btnValue14
			// 
			this->btnValue14->ImageIndex = 0;
			this->btnValue14->ImageList = this->imageList1;
			this->btnValue14->Location = System::Drawing::Point(384, 220);
			this->btnValue14->Name = S"btnValue14";
			this->btnValue14->Size = System::Drawing::Size(16, 14);
			this->btnValue14->TabIndex = 93;
			this->btnValue14->Tag = S"14";
			this->toolTip1->SetToolTip(this->btnValue14, S"Удалить");
			this->btnValue14->Click += new System::EventHandler(this, &FeatureFactoryGenerator::FeatureEditor::btnValue28_Click);
			// 
			// checkValue14
			// 
			this->checkValue14->Enabled = false;
			this->checkValue14->Location = System::Drawing::Point(408, 220);
			this->checkValue14->Name = S"checkValue14";
			this->checkValue14->Size = System::Drawing::Size(110, 16);
			this->checkValue14->TabIndex = 92;
			this->checkValue14->Text = S"не используется";
			this->toolTip1->SetToolTip(this->checkValue14, S"используется/не используется");
			this->checkValue14->CheckedChanged += new System::EventHandler(this, &FeatureFactoryGenerator::FeatureEditor::checkValue16_CheckedChanged);
			// 
			// btnValue13
			// 
			this->btnValue13->ImageIndex = 0;
			this->btnValue13->ImageList = this->imageList1;
			this->btnValue13->Location = System::Drawing::Point(384, 206);
			this->btnValue13->Name = S"btnValue13";
			this->btnValue13->Size = System::Drawing::Size(16, 14);
			this->btnValue13->TabIndex = 91;
			this->btnValue13->Tag = S"13";
			this->toolTip1->SetToolTip(this->btnValue13, S"Удалить");
			this->btnValue13->Click += new System::EventHandler(this, &FeatureFactoryGenerator::FeatureEditor::btnValue28_Click);
			// 
			// checkValue13
			// 
			this->checkValue13->Enabled = false;
			this->checkValue13->Location = System::Drawing::Point(408, 206);
			this->checkValue13->Name = S"checkValue13";
			this->checkValue13->Size = System::Drawing::Size(110, 16);
			this->checkValue13->TabIndex = 90;
			this->checkValue13->Text = S"не используется";
			this->toolTip1->SetToolTip(this->checkValue13, S"используется/не используется");
			this->checkValue13->CheckedChanged += new System::EventHandler(this, &FeatureFactoryGenerator::FeatureEditor::checkValue16_CheckedChanged);
			// 
			// btnValue12
			// 
			this->btnValue12->ImageIndex = 0;
			this->btnValue12->ImageList = this->imageList1;
			this->btnValue12->Location = System::Drawing::Point(384, 192);
			this->btnValue12->Name = S"btnValue12";
			this->btnValue12->Size = System::Drawing::Size(16, 14);
			this->btnValue12->TabIndex = 89;
			this->btnValue12->Tag = S"12";
			this->toolTip1->SetToolTip(this->btnValue12, S"Удалить");
			this->btnValue12->Click += new System::EventHandler(this, &FeatureFactoryGenerator::FeatureEditor::btnValue28_Click);
			// 
			// checkValue12
			// 
			this->checkValue12->Enabled = false;
			this->checkValue12->Location = System::Drawing::Point(408, 192);
			this->checkValue12->Name = S"checkValue12";
			this->checkValue12->Size = System::Drawing::Size(110, 16);
			this->checkValue12->TabIndex = 88;
			this->checkValue12->Text = S"не используется";
			this->toolTip1->SetToolTip(this->checkValue12, S"используется/не используется");
			this->checkValue12->CheckedChanged += new System::EventHandler(this, &FeatureFactoryGenerator::FeatureEditor::checkValue16_CheckedChanged);
			// 
			// btnValue11
			// 
			this->btnValue11->ImageIndex = 0;
			this->btnValue11->ImageList = this->imageList1;
			this->btnValue11->Location = System::Drawing::Point(384, 178);
			this->btnValue11->Name = S"btnValue11";
			this->btnValue11->Size = System::Drawing::Size(16, 14);
			this->btnValue11->TabIndex = 87;
			this->btnValue11->Tag = S"11";
			this->toolTip1->SetToolTip(this->btnValue11, S"Удалить");
			this->btnValue11->Click += new System::EventHandler(this, &FeatureFactoryGenerator::FeatureEditor::btnValue28_Click);
			// 
			// checkValue11
			// 
			this->checkValue11->Enabled = false;
			this->checkValue11->Location = System::Drawing::Point(408, 178);
			this->checkValue11->Name = S"checkValue11";
			this->checkValue11->Size = System::Drawing::Size(110, 16);
			this->checkValue11->TabIndex = 86;
			this->checkValue11->Text = S"не используется";
			this->toolTip1->SetToolTip(this->checkValue11, S"используется/не используется");
			this->checkValue11->CheckedChanged += new System::EventHandler(this, &FeatureFactoryGenerator::FeatureEditor::checkValue16_CheckedChanged);
			// 
			// btnValue10
			// 
			this->btnValue10->ImageIndex = 0;
			this->btnValue10->ImageList = this->imageList1;
			this->btnValue10->Location = System::Drawing::Point(384, 164);
			this->btnValue10->Name = S"btnValue10";
			this->btnValue10->Size = System::Drawing::Size(16, 14);
			this->btnValue10->TabIndex = 85;
			this->btnValue10->Tag = S"10";
			this->toolTip1->SetToolTip(this->btnValue10, S"Удалить");
			this->btnValue10->Click += new System::EventHandler(this, &FeatureFactoryGenerator::FeatureEditor::btnValue28_Click);
			// 
			// checkValue10
			// 
			this->checkValue10->Enabled = false;
			this->checkValue10->Location = System::Drawing::Point(408, 164);
			this->checkValue10->Name = S"checkValue10";
			this->checkValue10->Size = System::Drawing::Size(110, 16);
			this->checkValue10->TabIndex = 84;
			this->checkValue10->Text = S"не используется";
			this->toolTip1->SetToolTip(this->checkValue10, S"используется/не используется");
			this->checkValue10->CheckedChanged += new System::EventHandler(this, &FeatureFactoryGenerator::FeatureEditor::checkValue16_CheckedChanged);
			// 
			// btnValue9
			// 
			this->btnValue9->ImageIndex = 0;
			this->btnValue9->ImageList = this->imageList1;
			this->btnValue9->Location = System::Drawing::Point(384, 150);
			this->btnValue9->Name = S"btnValue9";
			this->btnValue9->Size = System::Drawing::Size(16, 14);
			this->btnValue9->TabIndex = 83;
			this->btnValue9->Tag = S"9";
			this->toolTip1->SetToolTip(this->btnValue9, S"Удалить");
			this->btnValue9->Click += new System::EventHandler(this, &FeatureFactoryGenerator::FeatureEditor::btnValue28_Click);
			// 
			// checkValue9
			// 
			this->checkValue9->Enabled = false;
			this->checkValue9->Location = System::Drawing::Point(408, 150);
			this->checkValue9->Name = S"checkValue9";
			this->checkValue9->Size = System::Drawing::Size(110, 16);
			this->checkValue9->TabIndex = 82;
			this->checkValue9->Text = S"не используется";
			this->toolTip1->SetToolTip(this->checkValue9, S"используется/не используется");
			this->checkValue9->CheckedChanged += new System::EventHandler(this, &FeatureFactoryGenerator::FeatureEditor::checkValue16_CheckedChanged);
			// 
			// btnValue8
			// 
			this->btnValue8->ImageIndex = 0;
			this->btnValue8->ImageList = this->imageList1;
			this->btnValue8->Location = System::Drawing::Point(384, 136);
			this->btnValue8->Name = S"btnValue8";
			this->btnValue8->Size = System::Drawing::Size(16, 14);
			this->btnValue8->TabIndex = 81;
			this->btnValue8->Tag = S"8";
			this->toolTip1->SetToolTip(this->btnValue8, S"Удалить");
			this->btnValue8->Click += new System::EventHandler(this, &FeatureFactoryGenerator::FeatureEditor::btnValue28_Click);
			// 
			// checkValue8
			// 
			this->checkValue8->Enabled = false;
			this->checkValue8->Location = System::Drawing::Point(408, 136);
			this->checkValue8->Name = S"checkValue8";
			this->checkValue8->Size = System::Drawing::Size(110, 16);
			this->checkValue8->TabIndex = 80;
			this->checkValue8->Text = S"не используется";
			this->toolTip1->SetToolTip(this->checkValue8, S"используется/не используется");
			this->checkValue8->CheckedChanged += new System::EventHandler(this, &FeatureFactoryGenerator::FeatureEditor::checkValue16_CheckedChanged);
			// 
			// btnValue7
			// 
			this->btnValue7->ImageIndex = 0;
			this->btnValue7->ImageList = this->imageList1;
			this->btnValue7->Location = System::Drawing::Point(384, 122);
			this->btnValue7->Name = S"btnValue7";
			this->btnValue7->Size = System::Drawing::Size(16, 14);
			this->btnValue7->TabIndex = 79;
			this->btnValue7->Tag = S"7";
			this->toolTip1->SetToolTip(this->btnValue7, S"Удалить");
			this->btnValue7->Click += new System::EventHandler(this, &FeatureFactoryGenerator::FeatureEditor::btnValue28_Click);
			// 
			// checkValue7
			// 
			this->checkValue7->Enabled = false;
			this->checkValue7->Location = System::Drawing::Point(408, 122);
			this->checkValue7->Name = S"checkValue7";
			this->checkValue7->Size = System::Drawing::Size(110, 16);
			this->checkValue7->TabIndex = 78;
			this->checkValue7->Text = S"не используется";
			this->toolTip1->SetToolTip(this->checkValue7, S"используется/не используется");
			this->checkValue7->CheckedChanged += new System::EventHandler(this, &FeatureFactoryGenerator::FeatureEditor::checkValue16_CheckedChanged);
			// 
			// btnValue6
			// 
			this->btnValue6->ImageIndex = 0;
			this->btnValue6->ImageList = this->imageList1;
			this->btnValue6->Location = System::Drawing::Point(384, 108);
			this->btnValue6->Name = S"btnValue6";
			this->btnValue6->Size = System::Drawing::Size(16, 14);
			this->btnValue6->TabIndex = 77;
			this->btnValue6->Tag = S"6";
			this->toolTip1->SetToolTip(this->btnValue6, S"Удалить");
			this->btnValue6->Click += new System::EventHandler(this, &FeatureFactoryGenerator::FeatureEditor::btnValue28_Click);
			// 
			// checkValue6
			// 
			this->checkValue6->Enabled = false;
			this->checkValue6->Location = System::Drawing::Point(408, 108);
			this->checkValue6->Name = S"checkValue6";
			this->checkValue6->Size = System::Drawing::Size(110, 16);
			this->checkValue6->TabIndex = 76;
			this->checkValue6->Text = S"не используется";
			this->toolTip1->SetToolTip(this->checkValue6, S"используется/не используется");
			this->checkValue6->CheckedChanged += new System::EventHandler(this, &FeatureFactoryGenerator::FeatureEditor::checkValue16_CheckedChanged);
			// 
			// btnValue5
			// 
			this->btnValue5->ImageIndex = 0;
			this->btnValue5->ImageList = this->imageList1;
			this->btnValue5->Location = System::Drawing::Point(384, 94);
			this->btnValue5->Name = S"btnValue5";
			this->btnValue5->Size = System::Drawing::Size(16, 14);
			this->btnValue5->TabIndex = 75;
			this->btnValue5->Tag = S"5";
			this->toolTip1->SetToolTip(this->btnValue5, S"Удалить");
			this->btnValue5->Click += new System::EventHandler(this, &FeatureFactoryGenerator::FeatureEditor::btnValue28_Click);
			// 
			// checkValue5
			// 
			this->checkValue5->Enabled = false;
			this->checkValue5->Location = System::Drawing::Point(408, 94);
			this->checkValue5->Name = S"checkValue5";
			this->checkValue5->Size = System::Drawing::Size(110, 16);
			this->checkValue5->TabIndex = 74;
			this->checkValue5->Text = S"не используется";
			this->toolTip1->SetToolTip(this->checkValue5, S"используется/не используется");
			this->checkValue5->CheckedChanged += new System::EventHandler(this, &FeatureFactoryGenerator::FeatureEditor::checkValue16_CheckedChanged);
			// 
			// btnValue4
			// 
			this->btnValue4->ImageIndex = 0;
			this->btnValue4->ImageList = this->imageList1;
			this->btnValue4->Location = System::Drawing::Point(384, 80);
			this->btnValue4->Name = S"btnValue4";
			this->btnValue4->Size = System::Drawing::Size(16, 14);
			this->btnValue4->TabIndex = 73;
			this->btnValue4->Tag = S"4";
			this->toolTip1->SetToolTip(this->btnValue4, S"Удалить");
			this->btnValue4->Click += new System::EventHandler(this, &FeatureFactoryGenerator::FeatureEditor::btnValue28_Click);
			// 
			// checkValue4
			// 
			this->checkValue4->Enabled = false;
			this->checkValue4->Location = System::Drawing::Point(408, 80);
			this->checkValue4->Name = S"checkValue4";
			this->checkValue4->Size = System::Drawing::Size(110, 16);
			this->checkValue4->TabIndex = 72;
			this->checkValue4->Text = S"не используется";
			this->toolTip1->SetToolTip(this->checkValue4, S"используется/не используется");
			this->checkValue4->CheckedChanged += new System::EventHandler(this, &FeatureFactoryGenerator::FeatureEditor::checkValue16_CheckedChanged);
			// 
			// btnValue3
			// 
			this->btnValue3->ImageIndex = 0;
			this->btnValue3->ImageList = this->imageList1;
			this->btnValue3->Location = System::Drawing::Point(384, 66);
			this->btnValue3->Name = S"btnValue3";
			this->btnValue3->Size = System::Drawing::Size(16, 14);
			this->btnValue3->TabIndex = 71;
			this->btnValue3->Tag = S"3";
			this->toolTip1->SetToolTip(this->btnValue3, S"Удалить");
			this->btnValue3->Click += new System::EventHandler(this, &FeatureFactoryGenerator::FeatureEditor::btnValue28_Click);
			// 
			// checkValue3
			// 
			this->checkValue3->Enabled = false;
			this->checkValue3->Location = System::Drawing::Point(408, 66);
			this->checkValue3->Name = S"checkValue3";
			this->checkValue3->Size = System::Drawing::Size(110, 16);
			this->checkValue3->TabIndex = 70;
			this->checkValue3->Text = S"не используется";
			this->toolTip1->SetToolTip(this->checkValue3, S"используется/не используется");
			this->checkValue3->CheckedChanged += new System::EventHandler(this, &FeatureFactoryGenerator::FeatureEditor::checkValue16_CheckedChanged);
			// 
			// btnValue2
			// 
			this->btnValue2->ImageIndex = 0;
			this->btnValue2->ImageList = this->imageList1;
			this->btnValue2->Location = System::Drawing::Point(384, 52);
			this->btnValue2->Name = S"btnValue2";
			this->btnValue2->Size = System::Drawing::Size(16, 14);
			this->btnValue2->TabIndex = 69;
			this->btnValue2->Tag = S"2";
			this->toolTip1->SetToolTip(this->btnValue2, S"Удалить");
			this->btnValue2->Click += new System::EventHandler(this, &FeatureFactoryGenerator::FeatureEditor::btnValue28_Click);
			// 
			// checkValue2
			// 
			this->checkValue2->Enabled = false;
			this->checkValue2->Location = System::Drawing::Point(408, 52);
			this->checkValue2->Name = S"checkValue2";
			this->checkValue2->Size = System::Drawing::Size(110, 16);
			this->checkValue2->TabIndex = 68;
			this->checkValue2->Text = S"не используется";
			this->toolTip1->SetToolTip(this->checkValue2, S"используется/не используется");
			this->checkValue2->CheckedChanged += new System::EventHandler(this, &FeatureFactoryGenerator::FeatureEditor::checkValue16_CheckedChanged);
			// 
			// btnValue1
			// 
			this->btnValue1->ImageIndex = 1;
			this->btnValue1->ImageList = this->imageList1;
			this->btnValue1->Location = System::Drawing::Point(384, 38);
			this->btnValue1->Name = S"btnValue1";
			this->btnValue1->Size = System::Drawing::Size(16, 14);
			this->btnValue1->TabIndex = 67;
			this->btnValue1->Tag = S"1";
			this->toolTip1->SetToolTip(this->btnValue1, S"Удалить");
			this->btnValue1->Click += new System::EventHandler(this, &FeatureFactoryGenerator::FeatureEditor::btnValue28_Click);
			// 
			// checkValue1
			// 
			this->checkValue1->Enabled = false;
			this->checkValue1->Location = System::Drawing::Point(408, 38);
			this->checkValue1->Name = S"checkValue1";
			this->checkValue1->Size = System::Drawing::Size(110, 16);
			this->checkValue1->TabIndex = 66;
			this->checkValue1->Text = S"не используется";
			this->toolTip1->SetToolTip(this->checkValue1, S"используется/не используется");
			this->checkValue1->CheckedChanged += new System::EventHandler(this, &FeatureFactoryGenerator::FeatureEditor::checkValue16_CheckedChanged);
			// 
			// btnValue0
			// 
			this->btnValue0->ImageIndex = 1;
			this->btnValue0->ImageList = this->imageList1;
			this->btnValue0->Location = System::Drawing::Point(384, 24);
			this->btnValue0->Name = S"btnValue0";
			this->btnValue0->Size = System::Drawing::Size(16, 14);
			this->btnValue0->TabIndex = 65;
			this->btnValue0->Tag = S"0";
			this->toolTip1->SetToolTip(this->btnValue0, S"Удалить");
			this->btnValue0->Click += new System::EventHandler(this, &FeatureFactoryGenerator::FeatureEditor::btnValue28_Click);
			// 
			// checkValue0
			// 
			this->checkValue0->Enabled = false;
			this->checkValue0->Location = System::Drawing::Point(408, 24);
			this->checkValue0->Name = S"checkValue0";
			this->checkValue0->Size = System::Drawing::Size(110, 16);
			this->checkValue0->TabIndex = 64;
			this->checkValue0->Text = S"не используется";
			this->toolTip1->SetToolTip(this->checkValue0, S"используется/не используется");
			this->checkValue0->CheckedChanged += new System::EventHandler(this, &FeatureFactoryGenerator::FeatureEditor::checkValue16_CheckedChanged);
			// 
			// textValueDesc31
			// 
			this->textValueDesc31->AutoSize = false;
			this->textValueDesc31->BackColor = System::Drawing::SystemColors::InactiveBorder;
			this->textValueDesc31->Enabled = false;
			this->textValueDesc31->Location = System::Drawing::Point(200, 458);
			this->textValueDesc31->Name = S"textValueDesc31";
			this->textValueDesc31->Size = System::Drawing::Size(176, 16);
			this->textValueDesc31->TabIndex = 63;
			this->textValueDesc31->Text = S"";
			this->toolTip1->SetToolTip(this->textValueDesc31, S"введите сюда описание значения характеристики");
			this->textValueDesc31->TextChanged += new System::EventHandler(this, &FeatureFactoryGenerator::FeatureEditor::textValueDesc16_TextChanged);
			// 
			// textValueDesc30
			// 
			this->textValueDesc30->AutoSize = false;
			this->textValueDesc30->BackColor = System::Drawing::SystemColors::InactiveBorder;
			this->textValueDesc30->Enabled = false;
			this->textValueDesc30->Location = System::Drawing::Point(200, 444);
			this->textValueDesc30->Name = S"textValueDesc30";
			this->textValueDesc30->Size = System::Drawing::Size(176, 16);
			this->textValueDesc30->TabIndex = 62;
			this->textValueDesc30->Text = S"";
			this->toolTip1->SetToolTip(this->textValueDesc30, S"введите сюда описание значения характеристики");
			this->textValueDesc30->TextChanged += new System::EventHandler(this, &FeatureFactoryGenerator::FeatureEditor::textValueDesc16_TextChanged);
			// 
			// textValueDesc29
			// 
			this->textValueDesc29->AutoSize = false;
			this->textValueDesc29->BackColor = System::Drawing::SystemColors::InactiveBorder;
			this->textValueDesc29->Enabled = false;
			this->textValueDesc29->Location = System::Drawing::Point(200, 430);
			this->textValueDesc29->Name = S"textValueDesc29";
			this->textValueDesc29->Size = System::Drawing::Size(176, 16);
			this->textValueDesc29->TabIndex = 61;
			this->textValueDesc29->Text = S"";
			this->toolTip1->SetToolTip(this->textValueDesc29, S"введите сюда описание значения характеристики");
			this->textValueDesc29->TextChanged += new System::EventHandler(this, &FeatureFactoryGenerator::FeatureEditor::textValueDesc16_TextChanged);
			// 
			// textValueDesc28
			// 
			this->textValueDesc28->AutoSize = false;
			this->textValueDesc28->BackColor = System::Drawing::SystemColors::InactiveBorder;
			this->textValueDesc28->Enabled = false;
			this->textValueDesc28->Location = System::Drawing::Point(200, 416);
			this->textValueDesc28->Name = S"textValueDesc28";
			this->textValueDesc28->Size = System::Drawing::Size(176, 16);
			this->textValueDesc28->TabIndex = 60;
			this->textValueDesc28->Text = S"";
			this->toolTip1->SetToolTip(this->textValueDesc28, S"введите сюда описание значения характеристики");
			this->textValueDesc28->TextChanged += new System::EventHandler(this, &FeatureFactoryGenerator::FeatureEditor::textValueDesc16_TextChanged);
			// 
			// textValueDesc27
			// 
			this->textValueDesc27->AutoSize = false;
			this->textValueDesc27->BackColor = System::Drawing::SystemColors::InactiveBorder;
			this->textValueDesc27->Enabled = false;
			this->textValueDesc27->Location = System::Drawing::Point(200, 402);
			this->textValueDesc27->Name = S"textValueDesc27";
			this->textValueDesc27->Size = System::Drawing::Size(176, 16);
			this->textValueDesc27->TabIndex = 59;
			this->textValueDesc27->Text = S"";
			this->toolTip1->SetToolTip(this->textValueDesc27, S"введите сюда описание значения характеристики");
			this->textValueDesc27->TextChanged += new System::EventHandler(this, &FeatureFactoryGenerator::FeatureEditor::textValueDesc16_TextChanged);
			// 
			// textValueDesc26
			// 
			this->textValueDesc26->AutoSize = false;
			this->textValueDesc26->BackColor = System::Drawing::SystemColors::InactiveBorder;
			this->textValueDesc26->Enabled = false;
			this->textValueDesc26->Location = System::Drawing::Point(200, 388);
			this->textValueDesc26->Name = S"textValueDesc26";
			this->textValueDesc26->Size = System::Drawing::Size(176, 16);
			this->textValueDesc26->TabIndex = 58;
			this->textValueDesc26->Text = S"";
			this->toolTip1->SetToolTip(this->textValueDesc26, S"введите сюда описание значения характеристики");
			this->textValueDesc26->TextChanged += new System::EventHandler(this, &FeatureFactoryGenerator::FeatureEditor::textValueDesc16_TextChanged);
			// 
			// textValueDesc25
			// 
			this->textValueDesc25->AutoSize = false;
			this->textValueDesc25->BackColor = System::Drawing::SystemColors::InactiveBorder;
			this->textValueDesc25->Enabled = false;
			this->textValueDesc25->Location = System::Drawing::Point(200, 374);
			this->textValueDesc25->Name = S"textValueDesc25";
			this->textValueDesc25->Size = System::Drawing::Size(176, 16);
			this->textValueDesc25->TabIndex = 57;
			this->textValueDesc25->Text = S"";
			this->toolTip1->SetToolTip(this->textValueDesc25, S"введите сюда описание значения характеристики");
			this->textValueDesc25->TextChanged += new System::EventHandler(this, &FeatureFactoryGenerator::FeatureEditor::textValueDesc16_TextChanged);
			// 
			// textValueDesc24
			// 
			this->textValueDesc24->AutoSize = false;
			this->textValueDesc24->BackColor = System::Drawing::SystemColors::InactiveBorder;
			this->textValueDesc24->Enabled = false;
			this->textValueDesc24->Location = System::Drawing::Point(200, 360);
			this->textValueDesc24->Name = S"textValueDesc24";
			this->textValueDesc24->Size = System::Drawing::Size(176, 16);
			this->textValueDesc24->TabIndex = 56;
			this->textValueDesc24->Text = S"";
			this->toolTip1->SetToolTip(this->textValueDesc24, S"введите сюда описание значения характеристики");
			this->textValueDesc24->TextChanged += new System::EventHandler(this, &FeatureFactoryGenerator::FeatureEditor::textValueDesc16_TextChanged);
			// 
			// textValueDesc23
			// 
			this->textValueDesc23->AutoSize = false;
			this->textValueDesc23->BackColor = System::Drawing::SystemColors::InactiveBorder;
			this->textValueDesc23->Enabled = false;
			this->textValueDesc23->Location = System::Drawing::Point(200, 346);
			this->textValueDesc23->Name = S"textValueDesc23";
			this->textValueDesc23->Size = System::Drawing::Size(176, 16);
			this->textValueDesc23->TabIndex = 55;
			this->textValueDesc23->Text = S"";
			this->toolTip1->SetToolTip(this->textValueDesc23, S"введите сюда описание значения характеристики");
			this->textValueDesc23->TextChanged += new System::EventHandler(this, &FeatureFactoryGenerator::FeatureEditor::textValueDesc16_TextChanged);
			// 
			// textValueDesc22
			// 
			this->textValueDesc22->AutoSize = false;
			this->textValueDesc22->BackColor = System::Drawing::SystemColors::InactiveBorder;
			this->textValueDesc22->Enabled = false;
			this->textValueDesc22->Location = System::Drawing::Point(200, 332);
			this->textValueDesc22->Name = S"textValueDesc22";
			this->textValueDesc22->Size = System::Drawing::Size(176, 16);
			this->textValueDesc22->TabIndex = 54;
			this->textValueDesc22->Text = S"";
			this->toolTip1->SetToolTip(this->textValueDesc22, S"введите сюда описание значения характеристики");
			this->textValueDesc22->TextChanged += new System::EventHandler(this, &FeatureFactoryGenerator::FeatureEditor::textValueDesc16_TextChanged);
			// 
			// textValueDesc21
			// 
			this->textValueDesc21->AutoSize = false;
			this->textValueDesc21->BackColor = System::Drawing::SystemColors::InactiveBorder;
			this->textValueDesc21->Enabled = false;
			this->textValueDesc21->Location = System::Drawing::Point(200, 318);
			this->textValueDesc21->Name = S"textValueDesc21";
			this->textValueDesc21->Size = System::Drawing::Size(176, 16);
			this->textValueDesc21->TabIndex = 53;
			this->textValueDesc21->Text = S"";
			this->toolTip1->SetToolTip(this->textValueDesc21, S"введите сюда описание значения характеристики");
			this->textValueDesc21->TextChanged += new System::EventHandler(this, &FeatureFactoryGenerator::FeatureEditor::textValueDesc16_TextChanged);
			// 
			// textValueDesc20
			// 
			this->textValueDesc20->AutoSize = false;
			this->textValueDesc20->BackColor = System::Drawing::SystemColors::InactiveBorder;
			this->textValueDesc20->Enabled = false;
			this->textValueDesc20->Location = System::Drawing::Point(200, 304);
			this->textValueDesc20->Name = S"textValueDesc20";
			this->textValueDesc20->Size = System::Drawing::Size(176, 16);
			this->textValueDesc20->TabIndex = 52;
			this->textValueDesc20->Text = S"";
			this->toolTip1->SetToolTip(this->textValueDesc20, S"введите сюда описание значения характеристики");
			this->textValueDesc20->TextChanged += new System::EventHandler(this, &FeatureFactoryGenerator::FeatureEditor::textValueDesc16_TextChanged);
			// 
			// textValueDesc19
			// 
			this->textValueDesc19->AutoSize = false;
			this->textValueDesc19->BackColor = System::Drawing::SystemColors::InactiveBorder;
			this->textValueDesc19->Enabled = false;
			this->textValueDesc19->Location = System::Drawing::Point(200, 290);
			this->textValueDesc19->Name = S"textValueDesc19";
			this->textValueDesc19->Size = System::Drawing::Size(176, 16);
			this->textValueDesc19->TabIndex = 51;
			this->textValueDesc19->Text = S"";
			this->toolTip1->SetToolTip(this->textValueDesc19, S"введите сюда описание значения характеристики");
			this->textValueDesc19->TextChanged += new System::EventHandler(this, &FeatureFactoryGenerator::FeatureEditor::textValueDesc16_TextChanged);
			// 
			// textValueDesc18
			// 
			this->textValueDesc18->AutoSize = false;
			this->textValueDesc18->BackColor = System::Drawing::SystemColors::InactiveBorder;
			this->textValueDesc18->Enabled = false;
			this->textValueDesc18->Location = System::Drawing::Point(200, 276);
			this->textValueDesc18->Name = S"textValueDesc18";
			this->textValueDesc18->Size = System::Drawing::Size(176, 16);
			this->textValueDesc18->TabIndex = 50;
			this->textValueDesc18->Text = S"";
			this->toolTip1->SetToolTip(this->textValueDesc18, S"введите сюда описание значения характеристики");
			this->textValueDesc18->TextChanged += new System::EventHandler(this, &FeatureFactoryGenerator::FeatureEditor::textValueDesc16_TextChanged);
			// 
			// textValueDesc17
			// 
			this->textValueDesc17->AutoSize = false;
			this->textValueDesc17->BackColor = System::Drawing::SystemColors::InactiveBorder;
			this->textValueDesc17->Enabled = false;
			this->textValueDesc17->Location = System::Drawing::Point(200, 262);
			this->textValueDesc17->Name = S"textValueDesc17";
			this->textValueDesc17->Size = System::Drawing::Size(176, 16);
			this->textValueDesc17->TabIndex = 49;
			this->textValueDesc17->Text = S"";
			this->toolTip1->SetToolTip(this->textValueDesc17, S"введите сюда описание значения характеристики");
			this->textValueDesc17->TextChanged += new System::EventHandler(this, &FeatureFactoryGenerator::FeatureEditor::textValueDesc16_TextChanged);
			// 
			// textValueDesc16
			// 
			this->textValueDesc16->AutoSize = false;
			this->textValueDesc16->BackColor = System::Drawing::SystemColors::InactiveBorder;
			this->textValueDesc16->Enabled = false;
			this->textValueDesc16->Location = System::Drawing::Point(200, 248);
			this->textValueDesc16->Name = S"textValueDesc16";
			this->textValueDesc16->Size = System::Drawing::Size(176, 16);
			this->textValueDesc16->TabIndex = 48;
			this->textValueDesc16->Text = S"";
			this->toolTip1->SetToolTip(this->textValueDesc16, S"введите сюда описание значения характеристики");
			this->textValueDesc16->TextChanged += new System::EventHandler(this, &FeatureFactoryGenerator::FeatureEditor::textValueDesc16_TextChanged);
			// 
			// textValueDesc15
			// 
			this->textValueDesc15->AutoSize = false;
			this->textValueDesc15->BackColor = System::Drawing::SystemColors::InactiveBorder;
			this->textValueDesc15->Enabled = false;
			this->textValueDesc15->Location = System::Drawing::Point(200, 234);
			this->textValueDesc15->Name = S"textValueDesc15";
			this->textValueDesc15->Size = System::Drawing::Size(176, 16);
			this->textValueDesc15->TabIndex = 47;
			this->textValueDesc15->Text = S"";
			this->toolTip1->SetToolTip(this->textValueDesc15, S"введите сюда описание значения характеристики");
			this->textValueDesc15->TextChanged += new System::EventHandler(this, &FeatureFactoryGenerator::FeatureEditor::textValueDesc16_TextChanged);
			// 
			// textValueDesc14
			// 
			this->textValueDesc14->AutoSize = false;
			this->textValueDesc14->BackColor = System::Drawing::SystemColors::InactiveBorder;
			this->textValueDesc14->Enabled = false;
			this->textValueDesc14->Location = System::Drawing::Point(200, 220);
			this->textValueDesc14->Name = S"textValueDesc14";
			this->textValueDesc14->Size = System::Drawing::Size(176, 16);
			this->textValueDesc14->TabIndex = 46;
			this->textValueDesc14->Text = S"";
			this->toolTip1->SetToolTip(this->textValueDesc14, S"введите сюда описание значения характеристики");
			this->textValueDesc14->TextChanged += new System::EventHandler(this, &FeatureFactoryGenerator::FeatureEditor::textValueDesc16_TextChanged);
			// 
			// textValueDesc13
			// 
			this->textValueDesc13->AutoSize = false;
			this->textValueDesc13->BackColor = System::Drawing::SystemColors::InactiveBorder;
			this->textValueDesc13->Enabled = false;
			this->textValueDesc13->Location = System::Drawing::Point(200, 206);
			this->textValueDesc13->Name = S"textValueDesc13";
			this->textValueDesc13->Size = System::Drawing::Size(176, 16);
			this->textValueDesc13->TabIndex = 45;
			this->textValueDesc13->Text = S"";
			this->toolTip1->SetToolTip(this->textValueDesc13, S"введите сюда описание значения характеристики");
			this->textValueDesc13->TextChanged += new System::EventHandler(this, &FeatureFactoryGenerator::FeatureEditor::textValueDesc16_TextChanged);
			// 
			// textValueDesc12
			// 
			this->textValueDesc12->AutoSize = false;
			this->textValueDesc12->BackColor = System::Drawing::SystemColors::InactiveBorder;
			this->textValueDesc12->Enabled = false;
			this->textValueDesc12->Location = System::Drawing::Point(200, 192);
			this->textValueDesc12->Name = S"textValueDesc12";
			this->textValueDesc12->Size = System::Drawing::Size(176, 16);
			this->textValueDesc12->TabIndex = 44;
			this->textValueDesc12->Text = S"";
			this->toolTip1->SetToolTip(this->textValueDesc12, S"введите сюда описание значения характеристики");
			this->textValueDesc12->TextChanged += new System::EventHandler(this, &FeatureFactoryGenerator::FeatureEditor::textValueDesc16_TextChanged);
			// 
			// textValueDesc11
			// 
			this->textValueDesc11->AutoSize = false;
			this->textValueDesc11->BackColor = System::Drawing::SystemColors::InactiveBorder;
			this->textValueDesc11->Enabled = false;
			this->textValueDesc11->Location = System::Drawing::Point(200, 178);
			this->textValueDesc11->Name = S"textValueDesc11";
			this->textValueDesc11->Size = System::Drawing::Size(176, 16);
			this->textValueDesc11->TabIndex = 43;
			this->textValueDesc11->Text = S"";
			this->toolTip1->SetToolTip(this->textValueDesc11, S"введите сюда описание значения характеристики");
			this->textValueDesc11->TextChanged += new System::EventHandler(this, &FeatureFactoryGenerator::FeatureEditor::textValueDesc16_TextChanged);
			// 
			// textValueDesc10
			// 
			this->textValueDesc10->AutoSize = false;
			this->textValueDesc10->BackColor = System::Drawing::SystemColors::InactiveBorder;
			this->textValueDesc10->Enabled = false;
			this->textValueDesc10->Location = System::Drawing::Point(200, 164);
			this->textValueDesc10->Name = S"textValueDesc10";
			this->textValueDesc10->Size = System::Drawing::Size(176, 16);
			this->textValueDesc10->TabIndex = 42;
			this->textValueDesc10->Text = S"";
			this->toolTip1->SetToolTip(this->textValueDesc10, S"введите сюда описание значения характеристики");
			this->textValueDesc10->TextChanged += new System::EventHandler(this, &FeatureFactoryGenerator::FeatureEditor::textValueDesc16_TextChanged);
			// 
			// textValueDesc9
			// 
			this->textValueDesc9->AutoSize = false;
			this->textValueDesc9->BackColor = System::Drawing::SystemColors::InactiveBorder;
			this->textValueDesc9->Enabled = false;
			this->textValueDesc9->Location = System::Drawing::Point(200, 150);
			this->textValueDesc9->Name = S"textValueDesc9";
			this->textValueDesc9->Size = System::Drawing::Size(176, 16);
			this->textValueDesc9->TabIndex = 41;
			this->textValueDesc9->Text = S"";
			this->toolTip1->SetToolTip(this->textValueDesc9, S"введите сюда описание значения характеристики");
			this->textValueDesc9->TextChanged += new System::EventHandler(this, &FeatureFactoryGenerator::FeatureEditor::textValueDesc16_TextChanged);
			// 
			// textValueDesc8
			// 
			this->textValueDesc8->AutoSize = false;
			this->textValueDesc8->BackColor = System::Drawing::SystemColors::InactiveBorder;
			this->textValueDesc8->Enabled = false;
			this->textValueDesc8->Location = System::Drawing::Point(200, 136);
			this->textValueDesc8->Name = S"textValueDesc8";
			this->textValueDesc8->Size = System::Drawing::Size(176, 16);
			this->textValueDesc8->TabIndex = 40;
			this->textValueDesc8->Text = S"";
			this->toolTip1->SetToolTip(this->textValueDesc8, S"введите сюда описание значения характеристики");
			this->textValueDesc8->TextChanged += new System::EventHandler(this, &FeatureFactoryGenerator::FeatureEditor::textValueDesc16_TextChanged);
			// 
			// textValueDesc7
			// 
			this->textValueDesc7->AutoSize = false;
			this->textValueDesc7->BackColor = System::Drawing::SystemColors::InactiveBorder;
			this->textValueDesc7->Enabled = false;
			this->textValueDesc7->Location = System::Drawing::Point(200, 122);
			this->textValueDesc7->Name = S"textValueDesc7";
			this->textValueDesc7->Size = System::Drawing::Size(176, 16);
			this->textValueDesc7->TabIndex = 39;
			this->textValueDesc7->Text = S"";
			this->toolTip1->SetToolTip(this->textValueDesc7, S"введите сюда описание значения характеристики");
			this->textValueDesc7->TextChanged += new System::EventHandler(this, &FeatureFactoryGenerator::FeatureEditor::textValueDesc16_TextChanged);
			// 
			// textValueDesc6
			// 
			this->textValueDesc6->AutoSize = false;
			this->textValueDesc6->BackColor = System::Drawing::SystemColors::InactiveBorder;
			this->textValueDesc6->Enabled = false;
			this->textValueDesc6->Location = System::Drawing::Point(200, 108);
			this->textValueDesc6->Name = S"textValueDesc6";
			this->textValueDesc6->Size = System::Drawing::Size(176, 16);
			this->textValueDesc6->TabIndex = 38;
			this->textValueDesc6->Text = S"";
			this->toolTip1->SetToolTip(this->textValueDesc6, S"введите сюда описание значения характеристики");
			this->textValueDesc6->TextChanged += new System::EventHandler(this, &FeatureFactoryGenerator::FeatureEditor::textValueDesc16_TextChanged);
			// 
			// textValueDesc5
			// 
			this->textValueDesc5->AutoSize = false;
			this->textValueDesc5->BackColor = System::Drawing::SystemColors::InactiveBorder;
			this->textValueDesc5->Enabled = false;
			this->textValueDesc5->Location = System::Drawing::Point(200, 94);
			this->textValueDesc5->Name = S"textValueDesc5";
			this->textValueDesc5->Size = System::Drawing::Size(176, 16);
			this->textValueDesc5->TabIndex = 37;
			this->textValueDesc5->Text = S"";
			this->toolTip1->SetToolTip(this->textValueDesc5, S"введите сюда описание значения характеристики");
			this->textValueDesc5->TextChanged += new System::EventHandler(this, &FeatureFactoryGenerator::FeatureEditor::textValueDesc16_TextChanged);
			// 
			// textValueDesc4
			// 
			this->textValueDesc4->AutoSize = false;
			this->textValueDesc4->BackColor = System::Drawing::SystemColors::InactiveBorder;
			this->textValueDesc4->Enabled = false;
			this->textValueDesc4->Location = System::Drawing::Point(200, 80);
			this->textValueDesc4->Name = S"textValueDesc4";
			this->textValueDesc4->Size = System::Drawing::Size(176, 16);
			this->textValueDesc4->TabIndex = 36;
			this->textValueDesc4->Text = S"";
			this->toolTip1->SetToolTip(this->textValueDesc4, S"введите сюда описание значения характеристики");
			this->textValueDesc4->TextChanged += new System::EventHandler(this, &FeatureFactoryGenerator::FeatureEditor::textValueDesc16_TextChanged);
			// 
			// textValueDesc3
			// 
			this->textValueDesc3->AutoSize = false;
			this->textValueDesc3->BackColor = System::Drawing::SystemColors::InactiveBorder;
			this->textValueDesc3->Enabled = false;
			this->textValueDesc3->Location = System::Drawing::Point(200, 66);
			this->textValueDesc3->Name = S"textValueDesc3";
			this->textValueDesc3->Size = System::Drawing::Size(176, 16);
			this->textValueDesc3->TabIndex = 35;
			this->textValueDesc3->Text = S"";
			this->toolTip1->SetToolTip(this->textValueDesc3, S"введите сюда описание значения характеристики");
			this->textValueDesc3->TextChanged += new System::EventHandler(this, &FeatureFactoryGenerator::FeatureEditor::textValueDesc16_TextChanged);
			// 
			// textValueDesc2
			// 
			this->textValueDesc2->AutoSize = false;
			this->textValueDesc2->BackColor = System::Drawing::SystemColors::InactiveBorder;
			this->textValueDesc2->Enabled = false;
			this->textValueDesc2->Location = System::Drawing::Point(200, 52);
			this->textValueDesc2->Name = S"textValueDesc2";
			this->textValueDesc2->Size = System::Drawing::Size(176, 16);
			this->textValueDesc2->TabIndex = 34;
			this->textValueDesc2->Text = S"";
			this->toolTip1->SetToolTip(this->textValueDesc2, S"введите сюда описание значения характеристики");
			this->textValueDesc2->TextChanged += new System::EventHandler(this, &FeatureFactoryGenerator::FeatureEditor::textValueDesc16_TextChanged);
			// 
			// textValueDesc1
			// 
			this->textValueDesc1->AutoSize = false;
			this->textValueDesc1->BackColor = System::Drawing::SystemColors::InactiveBorder;
			this->textValueDesc1->Enabled = false;
			this->textValueDesc1->Location = System::Drawing::Point(200, 38);
			this->textValueDesc1->Name = S"textValueDesc1";
			this->textValueDesc1->Size = System::Drawing::Size(176, 16);
			this->textValueDesc1->TabIndex = 33;
			this->textValueDesc1->Text = S"";
			this->toolTip1->SetToolTip(this->textValueDesc1, S"введите сюда описание значения характеристики");
			this->textValueDesc1->TextChanged += new System::EventHandler(this, &FeatureFactoryGenerator::FeatureEditor::textValueDesc16_TextChanged);
			// 
			// textValueDesc0
			// 
			this->textValueDesc0->AutoSize = false;
			this->textValueDesc0->BackColor = System::Drawing::SystemColors::Info;
			this->textValueDesc0->Enabled = false;
			this->textValueDesc0->Location = System::Drawing::Point(200, 24);
			this->textValueDesc0->Name = S"textValueDesc0";
			this->textValueDesc0->Size = System::Drawing::Size(176, 16);
			this->textValueDesc0->TabIndex = 32;
			this->textValueDesc0->Text = S"";
			this->toolTip1->SetToolTip(this->textValueDesc0, S"введите сюда описание значения характеристики");
			this->textValueDesc0->TextChanged += new System::EventHandler(this, &FeatureFactoryGenerator::FeatureEditor::textValueDesc16_TextChanged);
			// 
			// textValueName31
			// 
			this->textValueName31->AutoSize = false;
			this->textValueName31->BackColor = System::Drawing::SystemColors::InactiveBorder;
			this->textValueName31->Enabled = false;
			this->textValueName31->Font = new System::Drawing::Font(S"Courier New", 8.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, (System::Byte)204);
			this->textValueName31->ForeColor = System::Drawing::Color::Navy;
			this->textValueName31->Location = System::Drawing::Point(24, 458);
			this->textValueName31->Name = S"textValueName31";
			this->textValueName31->Size = System::Drawing::Size(176, 16);
			this->textValueName31->TabIndex = 31;
			this->textValueName31->Text = S"";
			this->toolTip1->SetToolTip(this->textValueName31, S"введите сюда название значения характеристики");
			this->textValueName31->TextChanged += new System::EventHandler(this, &FeatureFactoryGenerator::FeatureEditor::textValueDesc16_TextChanged);
			// 
			// textValueName30
			// 
			this->textValueName30->AutoSize = false;
			this->textValueName30->BackColor = System::Drawing::SystemColors::InactiveBorder;
			this->textValueName30->Enabled = false;
			this->textValueName30->Font = new System::Drawing::Font(S"Courier New", 8.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, (System::Byte)204);
			this->textValueName30->ForeColor = System::Drawing::Color::Navy;
			this->textValueName30->Location = System::Drawing::Point(24, 444);
			this->textValueName30->Name = S"textValueName30";
			this->textValueName30->Size = System::Drawing::Size(176, 16);
			this->textValueName30->TabIndex = 30;
			this->textValueName30->Text = S"";
			this->toolTip1->SetToolTip(this->textValueName30, S"введите сюда название значения характеристики");
			this->textValueName30->TextChanged += new System::EventHandler(this, &FeatureFactoryGenerator::FeatureEditor::textValueDesc16_TextChanged);
			// 
			// textValueName29
			// 
			this->textValueName29->AutoSize = false;
			this->textValueName29->BackColor = System::Drawing::SystemColors::InactiveBorder;
			this->textValueName29->Enabled = false;
			this->textValueName29->Font = new System::Drawing::Font(S"Courier New", 8.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, (System::Byte)204);
			this->textValueName29->ForeColor = System::Drawing::Color::Navy;
			this->textValueName29->Location = System::Drawing::Point(24, 430);
			this->textValueName29->Name = S"textValueName29";
			this->textValueName29->Size = System::Drawing::Size(176, 16);
			this->textValueName29->TabIndex = 29;
			this->textValueName29->Text = S"";
			this->toolTip1->SetToolTip(this->textValueName29, S"введите сюда название значения характеристики");
			this->textValueName29->TextChanged += new System::EventHandler(this, &FeatureFactoryGenerator::FeatureEditor::textValueDesc16_TextChanged);
			// 
			// textValueName28
			// 
			this->textValueName28->AutoSize = false;
			this->textValueName28->BackColor = System::Drawing::SystemColors::InactiveBorder;
			this->textValueName28->Enabled = false;
			this->textValueName28->Font = new System::Drawing::Font(S"Courier New", 8.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, (System::Byte)204);
			this->textValueName28->ForeColor = System::Drawing::Color::Navy;
			this->textValueName28->Location = System::Drawing::Point(24, 416);
			this->textValueName28->Name = S"textValueName28";
			this->textValueName28->Size = System::Drawing::Size(176, 16);
			this->textValueName28->TabIndex = 28;
			this->textValueName28->Text = S"";
			this->toolTip1->SetToolTip(this->textValueName28, S"введите сюда название значения характеристики");
			this->textValueName28->TextChanged += new System::EventHandler(this, &FeatureFactoryGenerator::FeatureEditor::textValueDesc16_TextChanged);
			// 
			// textValueName27
			// 
			this->textValueName27->AutoSize = false;
			this->textValueName27->BackColor = System::Drawing::SystemColors::InactiveBorder;
			this->textValueName27->Enabled = false;
			this->textValueName27->Font = new System::Drawing::Font(S"Courier New", 8.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, (System::Byte)204);
			this->textValueName27->ForeColor = System::Drawing::Color::Navy;
			this->textValueName27->Location = System::Drawing::Point(24, 402);
			this->textValueName27->Name = S"textValueName27";
			this->textValueName27->Size = System::Drawing::Size(176, 16);
			this->textValueName27->TabIndex = 27;
			this->textValueName27->Text = S"";
			this->toolTip1->SetToolTip(this->textValueName27, S"введите сюда название значения характеристики");
			this->textValueName27->TextChanged += new System::EventHandler(this, &FeatureFactoryGenerator::FeatureEditor::textValueDesc16_TextChanged);
			// 
			// textValueName26
			// 
			this->textValueName26->AutoSize = false;
			this->textValueName26->BackColor = System::Drawing::SystemColors::InactiveBorder;
			this->textValueName26->Enabled = false;
			this->textValueName26->Font = new System::Drawing::Font(S"Courier New", 8.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, (System::Byte)204);
			this->textValueName26->ForeColor = System::Drawing::Color::Navy;
			this->textValueName26->Location = System::Drawing::Point(24, 388);
			this->textValueName26->Name = S"textValueName26";
			this->textValueName26->Size = System::Drawing::Size(176, 16);
			this->textValueName26->TabIndex = 26;
			this->textValueName26->Text = S"";
			this->toolTip1->SetToolTip(this->textValueName26, S"введите сюда название значения характеристики");
			this->textValueName26->TextChanged += new System::EventHandler(this, &FeatureFactoryGenerator::FeatureEditor::textValueDesc16_TextChanged);
			// 
			// textValueName25
			// 
			this->textValueName25->AutoSize = false;
			this->textValueName25->BackColor = System::Drawing::SystemColors::InactiveBorder;
			this->textValueName25->Enabled = false;
			this->textValueName25->Font = new System::Drawing::Font(S"Courier New", 8.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, (System::Byte)204);
			this->textValueName25->ForeColor = System::Drawing::Color::Navy;
			this->textValueName25->Location = System::Drawing::Point(24, 374);
			this->textValueName25->Name = S"textValueName25";
			this->textValueName25->Size = System::Drawing::Size(176, 16);
			this->textValueName25->TabIndex = 25;
			this->textValueName25->Text = S"";
			this->toolTip1->SetToolTip(this->textValueName25, S"введите сюда название значения характеристики");
			this->textValueName25->TextChanged += new System::EventHandler(this, &FeatureFactoryGenerator::FeatureEditor::textValueDesc16_TextChanged);
			// 
			// textValueName24
			// 
			this->textValueName24->AutoSize = false;
			this->textValueName24->BackColor = System::Drawing::SystemColors::InactiveBorder;
			this->textValueName24->Enabled = false;
			this->textValueName24->Font = new System::Drawing::Font(S"Courier New", 8.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, (System::Byte)204);
			this->textValueName24->ForeColor = System::Drawing::Color::Navy;
			this->textValueName24->Location = System::Drawing::Point(24, 360);
			this->textValueName24->Name = S"textValueName24";
			this->textValueName24->Size = System::Drawing::Size(176, 16);
			this->textValueName24->TabIndex = 24;
			this->textValueName24->Text = S"";
			this->toolTip1->SetToolTip(this->textValueName24, S"введите сюда название значения характеристики");
			this->textValueName24->TextChanged += new System::EventHandler(this, &FeatureFactoryGenerator::FeatureEditor::textValueDesc16_TextChanged);
			// 
			// textValueName23
			// 
			this->textValueName23->AutoSize = false;
			this->textValueName23->BackColor = System::Drawing::SystemColors::InactiveBorder;
			this->textValueName23->Enabled = false;
			this->textValueName23->Font = new System::Drawing::Font(S"Courier New", 8.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, (System::Byte)204);
			this->textValueName23->ForeColor = System::Drawing::Color::Navy;
			this->textValueName23->Location = System::Drawing::Point(24, 346);
			this->textValueName23->Name = S"textValueName23";
			this->textValueName23->Size = System::Drawing::Size(176, 16);
			this->textValueName23->TabIndex = 23;
			this->textValueName23->Text = S"";
			this->toolTip1->SetToolTip(this->textValueName23, S"введите сюда название значения характеристики");
			this->textValueName23->TextChanged += new System::EventHandler(this, &FeatureFactoryGenerator::FeatureEditor::textValueDesc16_TextChanged);
			// 
			// textValueName22
			// 
			this->textValueName22->AutoSize = false;
			this->textValueName22->BackColor = System::Drawing::SystemColors::InactiveBorder;
			this->textValueName22->Enabled = false;
			this->textValueName22->Font = new System::Drawing::Font(S"Courier New", 8.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, (System::Byte)204);
			this->textValueName22->ForeColor = System::Drawing::Color::Navy;
			this->textValueName22->Location = System::Drawing::Point(24, 332);
			this->textValueName22->Name = S"textValueName22";
			this->textValueName22->Size = System::Drawing::Size(176, 16);
			this->textValueName22->TabIndex = 22;
			this->textValueName22->Text = S"";
			this->toolTip1->SetToolTip(this->textValueName22, S"введите сюда название значения характеристики");
			this->textValueName22->TextChanged += new System::EventHandler(this, &FeatureFactoryGenerator::FeatureEditor::textValueDesc16_TextChanged);
			// 
			// textValueName21
			// 
			this->textValueName21->AutoSize = false;
			this->textValueName21->BackColor = System::Drawing::SystemColors::InactiveBorder;
			this->textValueName21->Enabled = false;
			this->textValueName21->Font = new System::Drawing::Font(S"Courier New", 8.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, (System::Byte)204);
			this->textValueName21->ForeColor = System::Drawing::Color::Navy;
			this->textValueName21->Location = System::Drawing::Point(24, 318);
			this->textValueName21->Name = S"textValueName21";
			this->textValueName21->Size = System::Drawing::Size(176, 16);
			this->textValueName21->TabIndex = 21;
			this->textValueName21->Text = S"";
			this->toolTip1->SetToolTip(this->textValueName21, S"введите сюда название значения характеристики");
			this->textValueName21->TextChanged += new System::EventHandler(this, &FeatureFactoryGenerator::FeatureEditor::textValueDesc16_TextChanged);
			// 
			// textValueName20
			// 
			this->textValueName20->AutoSize = false;
			this->textValueName20->BackColor = System::Drawing::SystemColors::InactiveBorder;
			this->textValueName20->Enabled = false;
			this->textValueName20->Font = new System::Drawing::Font(S"Courier New", 8.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, (System::Byte)204);
			this->textValueName20->ForeColor = System::Drawing::Color::Navy;
			this->textValueName20->Location = System::Drawing::Point(24, 304);
			this->textValueName20->Name = S"textValueName20";
			this->textValueName20->Size = System::Drawing::Size(176, 16);
			this->textValueName20->TabIndex = 20;
			this->textValueName20->Text = S"";
			this->toolTip1->SetToolTip(this->textValueName20, S"введите сюда название значения характеристики");
			this->textValueName20->TextChanged += new System::EventHandler(this, &FeatureFactoryGenerator::FeatureEditor::textValueDesc16_TextChanged);
			// 
			// textValueName19
			// 
			this->textValueName19->AutoSize = false;
			this->textValueName19->BackColor = System::Drawing::SystemColors::InactiveBorder;
			this->textValueName19->Enabled = false;
			this->textValueName19->Font = new System::Drawing::Font(S"Courier New", 8.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, (System::Byte)204);
			this->textValueName19->ForeColor = System::Drawing::Color::Navy;
			this->textValueName19->Location = System::Drawing::Point(24, 290);
			this->textValueName19->Name = S"textValueName19";
			this->textValueName19->Size = System::Drawing::Size(176, 16);
			this->textValueName19->TabIndex = 19;
			this->textValueName19->Text = S"";
			this->toolTip1->SetToolTip(this->textValueName19, S"введите сюда название значения характеристики");
			this->textValueName19->TextChanged += new System::EventHandler(this, &FeatureFactoryGenerator::FeatureEditor::textValueDesc16_TextChanged);
			// 
			// textValueName18
			// 
			this->textValueName18->AutoSize = false;
			this->textValueName18->BackColor = System::Drawing::SystemColors::InactiveBorder;
			this->textValueName18->Enabled = false;
			this->textValueName18->Font = new System::Drawing::Font(S"Courier New", 8.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, (System::Byte)204);
			this->textValueName18->ForeColor = System::Drawing::Color::Navy;
			this->textValueName18->Location = System::Drawing::Point(24, 276);
			this->textValueName18->Name = S"textValueName18";
			this->textValueName18->Size = System::Drawing::Size(176, 16);
			this->textValueName18->TabIndex = 18;
			this->textValueName18->Text = S"";
			this->toolTip1->SetToolTip(this->textValueName18, S"введите сюда название значения характеристики");
			this->textValueName18->TextChanged += new System::EventHandler(this, &FeatureFactoryGenerator::FeatureEditor::textValueDesc16_TextChanged);
			// 
			// textValueName17
			// 
			this->textValueName17->AutoSize = false;
			this->textValueName17->BackColor = System::Drawing::SystemColors::InactiveBorder;
			this->textValueName17->Enabled = false;
			this->textValueName17->Font = new System::Drawing::Font(S"Courier New", 8.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, (System::Byte)204);
			this->textValueName17->ForeColor = System::Drawing::Color::Navy;
			this->textValueName17->Location = System::Drawing::Point(24, 262);
			this->textValueName17->Name = S"textValueName17";
			this->textValueName17->Size = System::Drawing::Size(176, 16);
			this->textValueName17->TabIndex = 17;
			this->textValueName17->Text = S"";
			this->toolTip1->SetToolTip(this->textValueName17, S"введите сюда название значения характеристики");
			this->textValueName17->TextChanged += new System::EventHandler(this, &FeatureFactoryGenerator::FeatureEditor::textValueDesc16_TextChanged);
			// 
			// textValueName16
			// 
			this->textValueName16->AutoSize = false;
			this->textValueName16->BackColor = System::Drawing::SystemColors::InactiveBorder;
			this->textValueName16->Enabled = false;
			this->textValueName16->Font = new System::Drawing::Font(S"Courier New", 8.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, (System::Byte)204);
			this->textValueName16->ForeColor = System::Drawing::Color::Navy;
			this->textValueName16->Location = System::Drawing::Point(24, 248);
			this->textValueName16->Name = S"textValueName16";
			this->textValueName16->Size = System::Drawing::Size(176, 16);
			this->textValueName16->TabIndex = 16;
			this->textValueName16->Text = S"";
			this->toolTip1->SetToolTip(this->textValueName16, S"введите сюда название значения характеристики");
			this->textValueName16->TextChanged += new System::EventHandler(this, &FeatureFactoryGenerator::FeatureEditor::textValueDesc16_TextChanged);
			// 
			// textValueName15
			// 
			this->textValueName15->AutoSize = false;
			this->textValueName15->BackColor = System::Drawing::SystemColors::InactiveBorder;
			this->textValueName15->Enabled = false;
			this->textValueName15->Font = new System::Drawing::Font(S"Courier New", 8.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, (System::Byte)204);
			this->textValueName15->ForeColor = System::Drawing::Color::Navy;
			this->textValueName15->Location = System::Drawing::Point(24, 234);
			this->textValueName15->Name = S"textValueName15";
			this->textValueName15->Size = System::Drawing::Size(176, 16);
			this->textValueName15->TabIndex = 15;
			this->textValueName15->Text = S"";
			this->toolTip1->SetToolTip(this->textValueName15, S"введите сюда название значения характеристики");
			this->textValueName15->TextChanged += new System::EventHandler(this, &FeatureFactoryGenerator::FeatureEditor::textValueDesc16_TextChanged);
			// 
			// textValueName14
			// 
			this->textValueName14->AutoSize = false;
			this->textValueName14->BackColor = System::Drawing::SystemColors::InactiveBorder;
			this->textValueName14->Enabled = false;
			this->textValueName14->Font = new System::Drawing::Font(S"Courier New", 8.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, (System::Byte)204);
			this->textValueName14->ForeColor = System::Drawing::Color::Navy;
			this->textValueName14->Location = System::Drawing::Point(24, 220);
			this->textValueName14->Name = S"textValueName14";
			this->textValueName14->Size = System::Drawing::Size(176, 16);
			this->textValueName14->TabIndex = 14;
			this->textValueName14->Text = S"";
			this->toolTip1->SetToolTip(this->textValueName14, S"введите сюда название значения характеристики");
			this->textValueName14->TextChanged += new System::EventHandler(this, &FeatureFactoryGenerator::FeatureEditor::textValueDesc16_TextChanged);
			// 
			// textValueName13
			// 
			this->textValueName13->AutoSize = false;
			this->textValueName13->BackColor = System::Drawing::SystemColors::InactiveBorder;
			this->textValueName13->Enabled = false;
			this->textValueName13->Font = new System::Drawing::Font(S"Courier New", 8.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, (System::Byte)204);
			this->textValueName13->ForeColor = System::Drawing::Color::Navy;
			this->textValueName13->Location = System::Drawing::Point(24, 206);
			this->textValueName13->Name = S"textValueName13";
			this->textValueName13->Size = System::Drawing::Size(176, 16);
			this->textValueName13->TabIndex = 13;
			this->textValueName13->Text = S"";
			this->toolTip1->SetToolTip(this->textValueName13, S"введите сюда название значения характеристики");
			this->textValueName13->TextChanged += new System::EventHandler(this, &FeatureFactoryGenerator::FeatureEditor::textValueDesc16_TextChanged);
			// 
			// textValueName12
			// 
			this->textValueName12->AutoSize = false;
			this->textValueName12->BackColor = System::Drawing::SystemColors::InactiveBorder;
			this->textValueName12->Enabled = false;
			this->textValueName12->Font = new System::Drawing::Font(S"Courier New", 8.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, (System::Byte)204);
			this->textValueName12->ForeColor = System::Drawing::Color::Navy;
			this->textValueName12->Location = System::Drawing::Point(24, 192);
			this->textValueName12->Name = S"textValueName12";
			this->textValueName12->Size = System::Drawing::Size(176, 16);
			this->textValueName12->TabIndex = 12;
			this->textValueName12->Text = S"";
			this->toolTip1->SetToolTip(this->textValueName12, S"введите сюда название значения характеристики");
			this->textValueName12->TextChanged += new System::EventHandler(this, &FeatureFactoryGenerator::FeatureEditor::textValueDesc16_TextChanged);
			// 
			// textValueName11
			// 
			this->textValueName11->AutoSize = false;
			this->textValueName11->BackColor = System::Drawing::SystemColors::InactiveBorder;
			this->textValueName11->Enabled = false;
			this->textValueName11->Font = new System::Drawing::Font(S"Courier New", 8.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, (System::Byte)204);
			this->textValueName11->ForeColor = System::Drawing::Color::Navy;
			this->textValueName11->Location = System::Drawing::Point(24, 178);
			this->textValueName11->Name = S"textValueName11";
			this->textValueName11->Size = System::Drawing::Size(176, 16);
			this->textValueName11->TabIndex = 11;
			this->textValueName11->Text = S"";
			this->toolTip1->SetToolTip(this->textValueName11, S"введите сюда название значения характеристики");
			this->textValueName11->TextChanged += new System::EventHandler(this, &FeatureFactoryGenerator::FeatureEditor::textValueDesc16_TextChanged);
			// 
			// textValueName10
			// 
			this->textValueName10->AutoSize = false;
			this->textValueName10->BackColor = System::Drawing::SystemColors::InactiveBorder;
			this->textValueName10->Enabled = false;
			this->textValueName10->Font = new System::Drawing::Font(S"Courier New", 8.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, (System::Byte)204);
			this->textValueName10->ForeColor = System::Drawing::Color::Navy;
			this->textValueName10->Location = System::Drawing::Point(24, 164);
			this->textValueName10->Name = S"textValueName10";
			this->textValueName10->Size = System::Drawing::Size(176, 16);
			this->textValueName10->TabIndex = 10;
			this->textValueName10->Text = S"";
			this->toolTip1->SetToolTip(this->textValueName10, S"введите сюда название значения характеристики");
			this->textValueName10->TextChanged += new System::EventHandler(this, &FeatureFactoryGenerator::FeatureEditor::textValueDesc16_TextChanged);
			// 
			// textValueName9
			// 
			this->textValueName9->AutoSize = false;
			this->textValueName9->BackColor = System::Drawing::SystemColors::InactiveBorder;
			this->textValueName9->Enabled = false;
			this->textValueName9->Font = new System::Drawing::Font(S"Courier New", 8.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, (System::Byte)204);
			this->textValueName9->ForeColor = System::Drawing::Color::Navy;
			this->textValueName9->Location = System::Drawing::Point(24, 150);
			this->textValueName9->Name = S"textValueName9";
			this->textValueName9->Size = System::Drawing::Size(176, 16);
			this->textValueName9->TabIndex = 9;
			this->textValueName9->Text = S"";
			this->toolTip1->SetToolTip(this->textValueName9, S"введите сюда название значения характеристики");
			this->textValueName9->TextChanged += new System::EventHandler(this, &FeatureFactoryGenerator::FeatureEditor::textValueDesc16_TextChanged);
			// 
			// textValueName8
			// 
			this->textValueName8->AutoSize = false;
			this->textValueName8->BackColor = System::Drawing::SystemColors::InactiveBorder;
			this->textValueName8->Enabled = false;
			this->textValueName8->Font = new System::Drawing::Font(S"Courier New", 8.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, (System::Byte)204);
			this->textValueName8->ForeColor = System::Drawing::Color::Navy;
			this->textValueName8->Location = System::Drawing::Point(24, 136);
			this->textValueName8->Name = S"textValueName8";
			this->textValueName8->Size = System::Drawing::Size(176, 16);
			this->textValueName8->TabIndex = 8;
			this->textValueName8->Text = S"";
			this->toolTip1->SetToolTip(this->textValueName8, S"введите сюда название значения характеристики");
			this->textValueName8->TextChanged += new System::EventHandler(this, &FeatureFactoryGenerator::FeatureEditor::textValueDesc16_TextChanged);
			// 
			// textValueName7
			// 
			this->textValueName7->AutoSize = false;
			this->textValueName7->BackColor = System::Drawing::SystemColors::InactiveBorder;
			this->textValueName7->Enabled = false;
			this->textValueName7->Font = new System::Drawing::Font(S"Courier New", 8.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, (System::Byte)204);
			this->textValueName7->ForeColor = System::Drawing::Color::Navy;
			this->textValueName7->Location = System::Drawing::Point(24, 122);
			this->textValueName7->Name = S"textValueName7";
			this->textValueName7->Size = System::Drawing::Size(176, 16);
			this->textValueName7->TabIndex = 7;
			this->textValueName7->Text = S"";
			this->toolTip1->SetToolTip(this->textValueName7, S"введите сюда название значения характеристики");
			this->textValueName7->TextChanged += new System::EventHandler(this, &FeatureFactoryGenerator::FeatureEditor::textValueDesc16_TextChanged);
			// 
			// textValueName6
			// 
			this->textValueName6->AutoSize = false;
			this->textValueName6->BackColor = System::Drawing::SystemColors::InactiveBorder;
			this->textValueName6->Enabled = false;
			this->textValueName6->Font = new System::Drawing::Font(S"Courier New", 8.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, (System::Byte)204);
			this->textValueName6->ForeColor = System::Drawing::Color::Navy;
			this->textValueName6->Location = System::Drawing::Point(24, 108);
			this->textValueName6->Name = S"textValueName6";
			this->textValueName6->Size = System::Drawing::Size(176, 16);
			this->textValueName6->TabIndex = 6;
			this->textValueName6->Text = S"";
			this->toolTip1->SetToolTip(this->textValueName6, S"введите сюда название значения характеристики");
			this->textValueName6->TextChanged += new System::EventHandler(this, &FeatureFactoryGenerator::FeatureEditor::textValueDesc16_TextChanged);
			// 
			// textValueName5
			// 
			this->textValueName5->AutoSize = false;
			this->textValueName5->BackColor = System::Drawing::SystemColors::InactiveBorder;
			this->textValueName5->Enabled = false;
			this->textValueName5->Font = new System::Drawing::Font(S"Courier New", 8.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, (System::Byte)204);
			this->textValueName5->ForeColor = System::Drawing::Color::Navy;
			this->textValueName5->Location = System::Drawing::Point(24, 94);
			this->textValueName5->Name = S"textValueName5";
			this->textValueName5->Size = System::Drawing::Size(176, 16);
			this->textValueName5->TabIndex = 5;
			this->textValueName5->Text = S"";
			this->toolTip1->SetToolTip(this->textValueName5, S"введите сюда название значения характеристики");
			this->textValueName5->TextChanged += new System::EventHandler(this, &FeatureFactoryGenerator::FeatureEditor::textValueDesc16_TextChanged);
			// 
			// textValueName4
			// 
			this->textValueName4->AutoSize = false;
			this->textValueName4->BackColor = System::Drawing::SystemColors::InactiveBorder;
			this->textValueName4->Enabled = false;
			this->textValueName4->Font = new System::Drawing::Font(S"Courier New", 8.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, (System::Byte)204);
			this->textValueName4->ForeColor = System::Drawing::Color::Navy;
			this->textValueName4->Location = System::Drawing::Point(24, 80);
			this->textValueName4->Name = S"textValueName4";
			this->textValueName4->Size = System::Drawing::Size(176, 16);
			this->textValueName4->TabIndex = 4;
			this->textValueName4->Text = S"";
			this->toolTip1->SetToolTip(this->textValueName4, S"введите сюда название значения характеристики");
			this->textValueName4->TextChanged += new System::EventHandler(this, &FeatureFactoryGenerator::FeatureEditor::textValueDesc16_TextChanged);
			// 
			// textValueName3
			// 
			this->textValueName3->AutoSize = false;
			this->textValueName3->BackColor = System::Drawing::SystemColors::InactiveBorder;
			this->textValueName3->Enabled = false;
			this->textValueName3->Font = new System::Drawing::Font(S"Courier New", 8.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, (System::Byte)204);
			this->textValueName3->ForeColor = System::Drawing::Color::Navy;
			this->textValueName3->Location = System::Drawing::Point(24, 66);
			this->textValueName3->Name = S"textValueName3";
			this->textValueName3->Size = System::Drawing::Size(176, 16);
			this->textValueName3->TabIndex = 3;
			this->textValueName3->Text = S"";
			this->toolTip1->SetToolTip(this->textValueName3, S"введите сюда название значения характеристики");
			this->textValueName3->TextChanged += new System::EventHandler(this, &FeatureFactoryGenerator::FeatureEditor::textValueDesc16_TextChanged);
			// 
			// textValueName2
			// 
			this->textValueName2->AutoSize = false;
			this->textValueName2->BackColor = System::Drawing::SystemColors::InactiveBorder;
			this->textValueName2->Enabled = false;
			this->textValueName2->Font = new System::Drawing::Font(S"Courier New", 8.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, (System::Byte)204);
			this->textValueName2->ForeColor = System::Drawing::Color::Navy;
			this->textValueName2->Location = System::Drawing::Point(24, 52);
			this->textValueName2->Name = S"textValueName2";
			this->textValueName2->Size = System::Drawing::Size(176, 16);
			this->textValueName2->TabIndex = 2;
			this->textValueName2->Text = S"";
			this->toolTip1->SetToolTip(this->textValueName2, S"введите сюда название значения характеристики");
			this->textValueName2->TextChanged += new System::EventHandler(this, &FeatureFactoryGenerator::FeatureEditor::textValueDesc16_TextChanged);
			// 
			// textValueName1
			// 
			this->textValueName1->AutoSize = false;
			this->textValueName1->BackColor = System::Drawing::SystemColors::InactiveBorder;
			this->textValueName1->Enabled = false;
			this->textValueName1->Font = new System::Drawing::Font(S"Courier New", 8.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, (System::Byte)204);
			this->textValueName1->ForeColor = System::Drawing::Color::Navy;
			this->textValueName1->Location = System::Drawing::Point(24, 38);
			this->textValueName1->Name = S"textValueName1";
			this->textValueName1->Size = System::Drawing::Size(176, 16);
			this->textValueName1->TabIndex = 1;
			this->textValueName1->Text = S"";
			this->toolTip1->SetToolTip(this->textValueName1, S"введите сюда название значения характеристики");
			this->textValueName1->TextChanged += new System::EventHandler(this, &FeatureFactoryGenerator::FeatureEditor::textValueDesc16_TextChanged);
			// 
			// textValueName0
			// 
			this->textValueName0->AutoSize = false;
			this->textValueName0->BackColor = System::Drawing::SystemColors::InactiveBorder;
			this->textValueName0->Enabled = false;
			this->textValueName0->Font = new System::Drawing::Font(S"Courier New", 8.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, (System::Byte)204);
			this->textValueName0->ForeColor = System::Drawing::Color::Navy;
			this->textValueName0->Location = System::Drawing::Point(24, 24);
			this->textValueName0->Name = S"textValueName0";
			this->textValueName0->Size = System::Drawing::Size(176, 16);
			this->textValueName0->TabIndex = 0;
			this->textValueName0->Text = S"";
			this->toolTip1->SetToolTip(this->textValueName0, S"введите сюда название значения характеристики");
			this->textValueName0->TextChanged += new System::EventHandler(this, &FeatureFactoryGenerator::FeatureEditor::textValueDesc16_TextChanged);
			// 
			// label5
			// 
			this->label5->Location = System::Drawing::Point(208, 88);
			this->label5->Name = S"label5";
			this->label5->Size = System::Drawing::Size(304, 16);
			this->label5->TabIndex = 5;
			this->label5->Text = S"Тип характеристики";
			// 
			// comboFeatureType
			// 
			this->comboFeatureType->DisplayMember = S"1";
			this->comboFeatureType->DropDownStyle = System::Windows::Forms::ComboBoxStyle::DropDownList;
			System::Object* __mcTemp__1[] = new System::Object*[3];
			__mcTemp__1[0] = S"MapFeature";
			__mcTemp__1[1] = S"Integer";
			__mcTemp__1[2] = S"Percent";
			this->comboFeatureType->Items->AddRange(__mcTemp__1);
			this->comboFeatureType->Location = System::Drawing::Point(16, 88);
			this->comboFeatureType->Name = S"comboFeatureType";
			this->comboFeatureType->Size = System::Drawing::Size(184, 21);
			this->comboFeatureType->TabIndex = 4;
			this->comboFeatureType->TextChanged += new System::EventHandler(this, &FeatureFactoryGenerator::FeatureEditor::textValueDesc16_TextChanged);
			this->comboFeatureType->SelectedIndexChanged += new System::EventHandler(this, &FeatureFactoryGenerator::FeatureEditor::comboFeatureType_SelectedIndexChanged);
			// 
			// label4
			// 
			this->label4->Location = System::Drawing::Point(344, 56);
			this->label4->Name = S"label4";
			this->label4->Size = System::Drawing::Size(152, 16);
			this->label4->TabIndex = 3;
			this->label4->Text = S"Описание характеристики";
			// 
			// textFeatureDescription
			// 
			this->textFeatureDescription->Location = System::Drawing::Point(16, 56);
			this->textFeatureDescription->Name = S"textFeatureDescription";
			this->textFeatureDescription->Size = System::Drawing::Size(320, 20);
			this->textFeatureDescription->TabIndex = 2;
			this->textFeatureDescription->Text = S"";
			this->textFeatureDescription->TextChanged += new System::EventHandler(this, &FeatureFactoryGenerator::FeatureEditor::textValueDesc16_TextChanged);
			// 
			// label3
			// 
			this->label3->Location = System::Drawing::Point(208, 24);
			this->label3->Name = S"label3";
			this->label3->Size = System::Drawing::Size(304, 16);
			this->label3->TabIndex = 1;
			this->label3->Text = S"Имя характеристики (только латинские буквы и цифры)";
			// 
			// textFeatureName
			// 
			this->textFeatureName->Font = new System::Drawing::Font(S"Courier New", 8.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, (System::Byte)204);
			this->textFeatureName->ForeColor = System::Drawing::Color::Navy;
			this->textFeatureName->Location = System::Drawing::Point(16, 24);
			this->textFeatureName->Name = S"textFeatureName";
			this->textFeatureName->Size = System::Drawing::Size(184, 20);
			this->textFeatureName->TabIndex = 0;
			this->textFeatureName->Text = S"";
			this->textFeatureName->TextChanged += new System::EventHandler(this, &FeatureFactoryGenerator::FeatureEditor::textValueDesc16_TextChanged);
			// 
			// labelPath
			// 
			this->labelPath->Location = System::Drawing::Point(208, 5);
			this->labelPath->Name = S"labelPath";
			this->labelPath->Size = System::Drawing::Size(640, 16);
			this->labelPath->TabIndex = 4;
			this->labelPath->Text = S"label3";
			// 
			// folderPath
			// 
			this->folderPath->RootFolder = System::Environment::SpecialFolder::MyComputer;
			this->folderPath->SelectedPath = S"E:\\working";
			// 
			// btnExitNoSave
			// 
			//---this->btnExitNoSave->Image = (__try_cast<System::Drawing::Image *  >(resources->GetObject(S"btnExitNoSave.Image")));
			this->btnExitNoSave->ImageAlign = System::Drawing::ContentAlignment::MiddleLeft;
			this->btnExitNoSave->Location = System::Drawing::Point(168, 696);
			this->btnExitNoSave->Name = S"btnExitNoSave";
			this->btnExitNoSave->Size = System::Drawing::Size(128, 32);
			this->btnExitNoSave->TabIndex = 7;
			this->btnExitNoSave->Text = S"Выйти без сохранения";
			this->btnExitNoSave->Click += new System::EventHandler(this, &FeatureFactoryGenerator::FeatureEditor::btnExitNoSave_Click);
			// 
			// btnExitAndSave
			// 
			//---this->btnExitAndSave->Image = (__try_cast<System::Drawing::Image *  >(resources->GetObject(S"btnExitAndSave.Image")));
			this->btnExitAndSave->ImageAlign = System::Drawing::ContentAlignment::MiddleLeft;
			this->btnExitAndSave->Location = System::Drawing::Point(24, 696);
			this->btnExitAndSave->Name = S"btnExitAndSave";
			this->btnExitAndSave->Size = System::Drawing::Size(128, 32);
			this->btnExitAndSave->TabIndex = 6;
			this->btnExitAndSave->Text = S"Сохранить и         выйти";
			this->btnExitAndSave->Click += new System::EventHandler(this, &FeatureFactoryGenerator::FeatureEditor::btnExitAndSave_Click);
			// 
			// pictureBox1
			// 
			//---this->pictureBox1->Image = (__try_cast<System::Drawing::Image *  >(resources->GetObject(S"pictureBox1.Image")));
			this->pictureBox1->Location = System::Drawing::Point(880, 24);
			this->pictureBox1->Name = S"pictureBox1";
			this->pictureBox1->Size = System::Drawing::Size(120, 700);
			this->pictureBox1->TabIndex = 8;
			this->pictureBox1->TabStop = false;
			// 
			// groupBox1
			// 
			this->groupBox1->Controls->Add(this->btnGenerate);
			this->groupBox1->Location = System::Drawing::Point(8, 640);
			this->groupBox1->Name = S"groupBox1";
			this->groupBox1->Size = System::Drawing::Size(304, 48);
			this->groupBox1->TabIndex = 9;
			this->groupBox1->TabStop = false;
			this->groupBox1->Text = S"Главная кнопка";
			// 
			// btnGenerate
			// 
			this->btnGenerate->Font = new System::Drawing::Font(S"Courier New", 12, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, (System::Byte)204);
			this->btnGenerate->ForeColor = System::Drawing::Color::Navy;
			this->btnGenerate->Location = System::Drawing::Point(16, 16);
			this->btnGenerate->Name = S"btnGenerate";
			this->btnGenerate->Size = System::Drawing::Size(272, 24);
			this->btnGenerate->TabIndex = 0;
			this->btnGenerate->Text = S"Сгенерить код С++";
			this->btnGenerate->Click += new System::EventHandler(this, &FeatureFactoryGenerator::FeatureEditor::btnGenerate_Click);
			// 
			// FeatureEditor
			// 
			this->AutoScaleBaseSize = System::Drawing::Size(5, 13);
			this->ClientSize = System::Drawing::Size(864, 743);
			this->Controls->Add(this->groupBox1);
			this->Controls->Add(this->pictureBox1);
			this->Controls->Add(this->btnExitNoSave);
			this->Controls->Add(this->btnExitAndSave);
			this->Controls->Add(this->labelPath);
			this->Controls->Add(this->groupFeatureEditor);
			this->Controls->Add(this->linkPath);
			this->Controls->Add(this->groupFeatureSelection);
			this->FormBorderStyle = System::Windows::Forms::FormBorderStyle::FixedSingle;
			this->MaximumSize = System::Drawing::Size(1024, 768);
			this->MinimumSize = System::Drawing::Size(870, 768);
			this->Name = S"FeatureEditor";
			this->Text = S"FeatureEditor";
			this->groupFeatureSelection->ResumeLayout(false);
			this->groupFeatureEditor->ResumeLayout(false);
			this->groupIntegerFeature->ResumeLayout(false);
			this->groupMapFeature->ResumeLayout(false);
			this->groupBox1->ResumeLayout(false);
			this->ResumeLayout(false);

		}		

		/// инициализирует все переменные для начала работы редактора
		void InitializeMy();

		///вносит изменения во внешний вид интерфейса после выбора другого типа характеристики
		void UpdateTypeChanging();

		///обновит вид интерфейса по вновь загруженным характеристикам
		void UpdateView();
		///обновит вид списка характеристик по номеру группы
		void UpdateGroupView(unsigned int nGroup);

		///обнуляет вид окна редактирования характеристики
		void ClearFeatureView();

		///выведет инфу о хар-ке в окошко редактирования
		void UpdateFeatureView(unsigned int nGroup, unsigned int nFeature);
		///выведет инфу о хар-ке из окошко редактирования в характеристику
		void UpdateFeatureFromView(unsigned int nGroup, unsigned int nFeature);
		///отобразит в окне редактирования характеристику
		void ShowFeature(CFeature* pFeature);
		///выведет инфу о хар-ке из окошко редактирования в характеристику
		void WriteFeature(CFeature* pFeature);

		///добавить/удалить мап-значение характеристики
		void AddRemoveFeatureValue(unsigned int nValue);

		///загружает с диска в память все характеристики из файлов ".ffgg" в выбранной директории
		void LoadFeatures();
		///ищет по указанному пути файлы ".ffgg" и формирует на их основе список групп характеристик
		void LoadFeatureGroups();

		///сохраняет на диск все характеристики из памяти в файлы ".ffgg" в выбранной директории
		void SaveFeatures();

		///добавит пустую новую хар-ку в текущую группу
		void AddFeature();
		///удалит выбранную хар-ку в текущей группе
		void DeleteFeature();


		///сгенерирует код С++
		void GenerateCode();


			///тут мы позволяем выбрать другую директорию для открытия списков хар-к
	private: System::Void linkPath_LinkClicked(System::Object *  sender, System::Windows::Forms::LinkLabelLinkClickedEventArgs *  e)
			 {
				 folderPath->ShowDialog();
				 labelPath->Text = folderPath->SelectedPath;
				 LoadFeatures();
			 }
			 ///когда меняется тип хар-ки в окне редактирования, мы отображаем нужное окошко редактирования
	private: System::Void comboFeatureType_SelectedIndexChanged(System::Object *  sender, System::EventArgs *  e)
			{
				UpdateTypeChanging();
				btnSaveFeature->Enabled = true;
			}

			///когда юзер выбирает новую группу характеристик, мы выводим список хар-к, входящих в нее
	private: System::Void comboFeatureGroup_SelectedIndexChanged(System::Object *  sender, System::EventArgs *  e)
			{
				UpdateGroupView(comboFeatureGroup->SelectedIndex);
				m_SelectedFeature = listFeatures->SelectedIndex;
			}

			///когда юзер выбирает характеристику из списка характеристик, мы отображаем ее в окне редактирования
	private: System::Void listFeatures_SelectedIndexChanged(System::Object *  sender, System::EventArgs *  e)
			{
//				System::Windows::Forms::ListBox* list = __try_cast<System::Windows::Forms::ListBox*>(sender);
				if(listFeatures->SelectedIndex != m_SelectedFeature){
					if(btnSaveFeature->Enabled){
						System::Windows::Forms::DialogResult result;

						result = System::Windows::Forms::MessageBox::Show(
							"Сохранить изменения в текущей характеристике?"
							, "Смена текущей характеритсики"
							, System::Windows::Forms::MessageBoxButtons::YesNo
							, System::Windows::Forms::MessageBoxIcon::Question);
						if(result == DialogResult::Yes){
							btnSaveFeature_Click(NULL, NULL);
						}
					}

					UpdateFeatureView(comboFeatureGroup->SelectedIndex, listFeatures->SelectedIndex);
					m_SelectedFeature = listFeatures->SelectedIndex;
				}
			}

private: System::Void checkValue16_CheckedChanged(System::Object *  sender, System::EventArgs *  e)
		 {
			 System::Windows::Forms::CheckBox* chk;
			 chk = __try_cast<System::Windows::Forms::CheckBox*>(sender);
			 if(chk->Checked){
				 chk->Text = "используется";
			 }
			 else{
				 chk->Text = "не используется";
			 }
			 btnSaveFeature->Enabled = true;
		 }

private: System::Void textValueDesc16_TextChanged(System::Object *  sender, System::EventArgs *  e)
		 {
			 btnSaveFeature->Enabled = true;
		 }

private: System::Void btnUndoChanges_Click(System::Object *  sender, System::EventArgs *  e)
		 {
			UpdateGroupView(comboFeatureGroup->SelectedIndex);
			listFeatures->SelectedIndex = m_SelectedFeature;
			UpdateFeatureView(comboFeatureGroup->SelectedIndex, listFeatures->SelectedIndex);
		 }

private: System::Void btnSaveFeature_Click(System::Object *  sender, System::EventArgs *  e)
		 {
			UpdateFeatureFromView(comboFeatureGroup->SelectedIndex, listFeatures->SelectedIndex);
			UpdateGroupView(comboFeatureGroup->SelectedIndex);
			listFeatures->SelectedIndex = m_SelectedFeature;
			UpdateFeatureView(comboFeatureGroup->SelectedIndex, listFeatures->SelectedIndex);
		 }

private: System::Void btnValue28_Click(System::Object *  sender, System::EventArgs *  e)
		 {
			 AddRemoveFeatureValue((__try_cast<System::Windows::Forms::Button*>(sender))->Tag->ToString()->ToUInt32(NULL));
		 }

private: System::Void btnAddFeature_Click(System::Object *  sender, System::EventArgs *  e)
		 {
			 AddFeature();
		 }

private: System::Void btnDeleteFeature_Click(System::Object *  sender, System::EventArgs *  e)
		 {
			 DeleteFeature();
		 }

private: System::Void btnExitNoSave_Click(System::Object *  sender, System::EventArgs *  e)
		 {
			 Application::Exit();
		 }

private: System::Void btnExitAndSave_Click(System::Object *  sender, System::EventArgs *  e)
		 {
			 SaveFeatures();
			 Application::Exit();
		 }

private: System::Void btnGenerate_Click(System::Object *  sender, System::EventArgs *  e)
		 {
			System::Windows::Forms::DialogResult result;

			result = System::Windows::Forms::MessageBox::Show(
				"Все изменения в текущей базе характеристик будут автоматически сохранены. Продолжить?"
				, "Генерация кода"
				, System::Windows::Forms::MessageBoxButtons::YesNo
				, System::Windows::Forms::MessageBoxIcon::Question);
			if(result == DialogResult::Yes){
				btnSaveFeature_Click(NULL, NULL);
				SaveFeatures();
				GenerateCode();
			}
		 }

};
}