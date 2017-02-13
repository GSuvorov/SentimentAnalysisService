#pragma once

#include ".\macrodefinitiongenerator.h"

namespace FeatureFactoryGenerator
{
	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;

	/// <summary> 
	/// Summary for Form1
	///
	/// WARNING: If you change the name of this class, you will need to change the 
	///          'Resource File Name' property for the managed resource compiler tool 
	///          associated with all .resx files this class depends on.  Otherwise,
	///          the designers will not be able to interact properly with localized
	///          resources associated with this form.
	/// </summary>
	public __gc class Form1 : public System::Windows::Forms::Form
	{	
	public:
		Form1(void)
		{
			InitializeComponent();
			m_pMacroDefinitionGenerator = new CMacroDefinitionGenerator();
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

	private: System::Windows::Forms::Button *  button1;

	private: CMacroDefinitionGenerator* m_pMacroDefinitionGenerator;
	private: System::Windows::Forms::TextBox *  textMacro1;
	private: System::Windows::Forms::RichTextBox *  textMacro;






	private:
		/// <summary>
		/// Required designer variable.
		/// </summary>
		System::ComponentModel::Container * components;

		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		void InitializeComponent(void)
		{
			this->textMacro1 = new System::Windows::Forms::TextBox();
			this->button1 = new System::Windows::Forms::Button();
			this->textMacro = new System::Windows::Forms::RichTextBox();
			this->SuspendLayout();
			// 
			// textMacro1
			// 
			this->textMacro1->AcceptsReturn = true;
			this->textMacro1->AcceptsTab = true;
			this->textMacro1->AutoSize = false;
			this->textMacro1->BackColor = System::Drawing::SystemColors::ActiveBorder;
			this->textMacro1->Font = new System::Drawing::Font(S"Courier New", 9, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, (System::Byte)204);
			this->textMacro1->Location = System::Drawing::Point(560, 8);
			this->textMacro1->MaxLength = 327670;
			this->textMacro1->Multiline = true;
			this->textMacro1->Name = S"textMacro1";
			this->textMacro1->ScrollBars = System::Windows::Forms::ScrollBars::Both;
			this->textMacro1->Size = System::Drawing::Size(304, 600);
			this->textMacro1->TabIndex = 0;
			this->textMacro1->Text = S"#define MAP_FEATURE(\\\r\n\tFeatureName,\\\r\n\tBitSize,\\\r\n\tname_@,\\\r\n\tdesc_@) \\\r\n\ttypede" 
				S"f CAMTMap<11, 11> FeatureName##_value;\\\r\n\tclass FeatureName\\\r\n\t{\\\r\n\tpublic:\\\r\n\t\t" 
				S"FeatureName##_value name_1;\\\r\n\t\tFeatureName##_value name_2;\\\r\n\t\tFeatureName(void" 
				S")\\\r\n\t\t\t: name_1(0x00000001, desc_1)\\\r\n\t\t\t, name_2(0x00000002, desc_2)\\\r\n\t\t{\\\r\n\t\t" 
				S"}\\\r\n\t\t~FeatureName(void){};\\\r\n\t};\r\n";
			this->textMacro1->WordWrap = false;
			// 
			// button1
			// 
			this->button1->Location = System::Drawing::Point(56, 16);
			this->button1->Name = S"button1";
			this->button1->Size = System::Drawing::Size(144, 23);
			this->button1->TabIndex = 1;
			this->button1->Text = S"Make Templates";
			this->button1->Click += new System::EventHandler(this, &FeatureFactoryGenerator::Form1::button1_Click);
			// 
			// textMacro
			// 
			this->textMacro->AcceptsTab = true;
			this->textMacro->BackColor = System::Drawing::SystemColors::InactiveBorder;
			this->textMacro->DetectUrls = false;
			this->textMacro->Font = new System::Drawing::Font(S"Courier New", 9, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, (System::Byte)204);
			this->textMacro->Location = System::Drawing::Point(216, 0);
			this->textMacro->Name = S"textMacro";
			this->textMacro->Size = System::Drawing::Size(328, 600);
			this->textMacro->TabIndex = 2;
			this->textMacro->Text = S"richTextBox1";
			this->textMacro->WordWrap = false;
			// 
			// Form1
			// 
			this->AutoScaleBaseSize = System::Drawing::Size(5, 13);
			this->ClientSize = System::Drawing::Size(872, 613);
			this->Controls->Add(this->textMacro);
			this->Controls->Add(this->button1);
			this->Controls->Add(this->textMacro1);
			this->Name = S"Form1";
			this->Text = S"Form1";
			this->ResumeLayout(false);

		}	

		
	private: System::Void button1_Click(System::Object *  sender, System::EventArgs *  e)
			 {
				 System::String* s = new System::String(m_pMacroDefinitionGenerator->MakeTemplates());
				 System::IO::StringReader* sr = new System::IO::StringReader(s);
				 s = sr->ReadLine();
				 
				 textMacro->SuspendLayout();
				 //textMacro->Hide();
				 textMacro->Clear();
				 System::String* ss = s;
				 while(s){
					textMacro->AppendText(s);
					textMacro->AppendText("\n");
					s = sr->ReadLine();
				 }
				 //textMacro->Show();
				 textMacro->ResumeLayout();
			 }

	};
}


