#pragma once

//#include <vector>
//#include <string>
#include ".\featurevalue.h"
namespace FeatureFactoryGenerator
{
	public __gc class CFeature
	{
	public:
		CFeature(CFeature* pFeature);

		CFeature(
			System::String* sFeatureName
			, System::String* sFeatureDescription
			, System::String* sFeatureType
			, System::String* sFeatureTypeDescription);

		~CFeature(void);

		System::String* GetFeatureName(){return m_sFeatureName;};
		System::String* GetFeatureDescription(){return m_sFeatureDescription;};
		System::String* GetFeatureType(){return m_sFeatureType;};
		System::String* GetFeatureTypeDescription(){return m_sFeatureTypeDescription;};

		void SetFeatureName(System::String* value){m_sFeatureName = value;};
		void SetFeatureDescription(System::String* value){m_sFeatureDescription = value;};
		void SetFeatureType(System::String* value){m_sFeatureType = value;};
		void SetFeatureTypeDescription(System::String* value){m_sFeatureTypeDescription = value;};

		///распарсит свои значения из строки
		void LoadValues(System::String* sFeature);
		///3апарсит свои значения B строкy
		System::String* Write(System::String* sText);

		///очистит список мап-значений
		void ClearMapValues();
		///добавит новое значение в мап
		CFeatureValue* AddMapValue(System::String* sValueName, System::String* sValueDescription, bool bEnabled);
		///удалит мап-значение под определенным номером
		void DeleteFeatureValue(unsigned int nFeatureValue);

		unsigned int GetValueCount(){return m_ValueArray->Count;};

		///вернет указатель на характеристику 
		CFeatureValue* GetFeatureValue(unsigned int nFeatureValue);
		///вернет указатель на первую характеристику 
		CFeatureValue* FirstValue();
		///вернет указатель на очередную характеристику 
		CFeatureValue* NextValue();
	protected:
		System::String* m_sFeatureName;
		System::String* m_sFeatureDescription;
		System::String* m_sFeatureType;
		System::String* m_sFeatureTypeDescription;

		///набор значений, входящих в мап-характеристику
		System::Collections::ArrayList* m_ValueArray;
		///итератор на текущее значение
		System::Collections::IEnumerator* m_ArrayEnum;
	};
}