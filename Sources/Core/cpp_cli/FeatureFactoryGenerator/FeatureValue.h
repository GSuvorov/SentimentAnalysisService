#pragma once

namespace FeatureFactoryGenerator
{
	public __gc class CFeatureValue
	{
	public:
		CFeatureValue(System::String* sValueName, System::String* sValueDescription, bool bEnabled);
		~CFeatureValue(void);

		System::String* GetValueName(){return m_sValueName;};
		System::String* GetValueDescription(){return m_sValueDescription;};
		bool IsEnabled(){return m_bEnabled;};
		///3апарсит свои значения B строкy
		System::String* Write(System::String* sText);
	protected:
		System::String* m_sValueName;
		System::String* m_sValueDescription;
		bool m_bEnabled;
	};
}