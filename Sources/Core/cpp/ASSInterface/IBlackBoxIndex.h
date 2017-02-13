#pragma once

#include "IBlackBoxFeature.h"
#include "IBlackBoxDictionaryIndex.h"


namespace SS
{
	namespace Interface
	{
		namespace Core
		{
			namespace BlackBox
			{
/// инкапсулятор параметров отдельного слова
/** инкапсулятор параметров отдельного слова, отличного по индексу,
	предназначенному для индексации/поиска
*/
				class IIndex
				{
				public:
					///установление значения флага индексации
					virtual void SetIndexationFlag(bool bl) = 0;
					///получение значения флага индексации
					virtual bool GetIndexationFlag() = 0;

					///получение словарного индекса 
					virtual IDictionaryIndex* GetDictionaryIndex() = 0;

					///добавление характеристик
					virtual void AddFeature(IFeature* pFeature) = 0;
					///получение первой характеристики
					virtual IFeature* GetFirstFeature() = 0;
					///получение следующей характеристики
					virtual IFeature* GetNextFeature() = 0;

					///достает индекс из дерева, при этом не убивая ничего, вернет ссылку на индекс справа
					virtual IIndex* Pop() = 0;
					///удаление индекса
					virtual IIndex* ReleaseIndex() = 0;
					///очистка всего индекса, включая очистку набора характеристик
					virtual void ClearIndex() = 0;
				};
			}
		}
	}
}
