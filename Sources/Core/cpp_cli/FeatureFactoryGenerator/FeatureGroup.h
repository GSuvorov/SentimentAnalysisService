#pragma once

#include "Feature.h"
namespace FeatureFactoryGenerator
{
	public __gc class CFeatureGroup
	{
	public:
		CFeatureGroup(System::String* sGroupName);
		~CFeatureGroup(void);

		///вернет имя группы
		System::String* GetGroupName(){return m_sGroupName;};

		///загружает характеристики из файлa в директории sPath
		void LoadFeatures(System::String* sPath);
		///сохраняет характеристики в файл в директории sPath
		void SaveFeatures(System::String* sPath);

		///добавляет новую характеристику
		CFeature* AddFeature(
			System::String* sFeatureName
			, System::String* sFeatureDescription
			, System::String* sFeatureType
			, System::String* sFeatureTypeDescription);

		///удалит характеристику  под определенным номером
		void DeleteFeature(unsigned int nFeature);
		///вернет указатель на характеристику 
		CFeature* GetFeature(unsigned int nFeature);
		///вернет указатель на первую характеристику 
		CFeature* First();
		///вернет указатель на очередную характеристику 
		CFeature* Next();
	protected:
		///набор характеристик, входящих в группу
		System::Collections::ArrayList* m_FeatureArray;
		///итератор на текущую характеристику
		System::Collections::IEnumerator* m_ArrayEnum;

		///имя группы
		System::String* m_sGroupName;
	};
}