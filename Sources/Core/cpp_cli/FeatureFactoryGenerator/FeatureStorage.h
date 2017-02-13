#pragma once

#include "FeatureGroup.h"
namespace FeatureFactoryGenerator
{
	public __gc class CFeatureStorage
	{
	public:
		CFeatureStorage(void);
		~CFeatureStorage(void);
		///загружает характеристики из файлов в директории sPath
		void LoadGroups(System::String* sPath);
		///сохраняет характеристики в файлы в директории sPath
		void SaveGroups(System::String* sPath);

		///добавит пустую группу характеристик с заданным именем
		CFeatureGroup* AddGroup(System::String* sGroupName);

		///вернет указатель на группу 
		CFeatureGroup* GetGroup(unsigned int nGroup);

		///вернет указатель на первую группу 
		CFeatureGroup* FirstGroup();
		///вернет указатель на очередную группу 
		CFeatureGroup* NextGroup();
	protected:
		///набор групп характеристик
		//std::vector<CFeatureGroup*> m_v_FeatureGroups;
		System::Collections::ArrayList* m_FeatureGroupArray;
		//CFeatureGroup* m_FeatureGroupArray[];

		///итератор на текущую группу
		System::Collections::IEnumerator* m_ArrayEnum;

		///хранит путь, по которому грузились хар-ки (для последующего сохранения)
		System::String* m_sPath;
	};
}