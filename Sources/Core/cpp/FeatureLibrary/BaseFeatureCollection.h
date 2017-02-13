#pragma once

#include "BaseFeature.h"
#include <list>
#include <vector>
namespace SS
{
	namespace Core
	{
		namespace Features
		{
			namespace Base
			{

				///тип набора (для контроля допустимости проведения операций)
				enum TFeatureCollectionType
				{
					///неизвестный(базовый)
					fctUnknown,
					///CMorphoFeature
					fctMorphoFeature,
					///CMorphoFeatureApriori
					fctMorphoFeatureApriori,
					///CMorphoFeatureAposteriori
					fctMorphoFeatureAposteriori,
					///CSemanticFeature
					fctSemanticFeature,
					///CSyntaxFeatureApriori
					fctSyntaxFeatureApriori,
					///CSyntaxFeatureAposteriori
					fctSyntaxFeatureAposteriori,
					///CObjectFeature
					fctConstructionFeature,
					///CSentenceFeature
					fctSentenceFeature,
				///CSemanticFeatureApriori
				fctSemanticFeatureApriori,
				///CSemanticFeatureAposteriori
				fctSemanticFeatureAposteriori,

				};

				/// базовый класс для всех наборов характеристик. содержит также эталонную(математическую) реализацию базовых операций над набором
				class CBaseFeatureCollection
				{
				public:
					CBaseFeatureCollection(void);
					virtual ~CBaseFeatureCollection(void);

					///получение строкового описания значения хар-ки
					virtual void GetXMLValue(std::string& sXML) const;
					///установление значения хар-ки по строковому описанию
					virtual void SetXMLValue(const std::string& sXML);

					///получение строкового описания значения хар-к
					virtual void ToString(std::string& s) const;
					///получение строкового описания значения хар-к
					std::string CBaseFeatureCollection::ToString() const;

					/// сбрасывет значение своих переменных
					virtual	void Undefine();
					///вернет тру если все хар-ки Undefined
					virtual bool IsUndefined() const;

					/// возвращает вектор битов, для последующей записи на диск и возможного восстановления информации
					virtual	void GetValue(std::vector<unsigned char>& vecOfByte);
					///Тестовый метод
					void GetValueTest(std::vector<unsigned char> & vecOfByte);
					///записывает вектор битов начиная с переданной позиции, позволяющий восстановить информацию о классе, смещает текущую позицию на число бит, занимаемое классом
					virtual	void SetValue(std::vector<unsigned char>& vecOfByte, unsigned int& uiCurPosition);

					/// "оператор = " - присваивает значение другого класса такого-же типа
					virtual void Assign(const CBaseFeatureCollection* pValue);

					/// "оператор == " - сравнивает со значением другого класса такого-же типа
					virtual bool Equal(const CBaseFeatureCollection* pValue)  const;
					/// "оператор &= " - присваивает значение другого класса такого-же типа
					virtual void AssignAND(const CBaseFeatureCollection* pValue);
					/// "оператор |= " - присваивает значение другого класса такого-же типа
					virtual void AssignOR(const CBaseFeatureCollection* pValue);

					/// математическое включение множеств (включает ли данное множество переданное множество)
					virtual bool IsIncludes(const CBaseFeatureCollection* pValue) const;

					/// математическое пересечение множеств (пересекается ли данное множество с переданным множеством)
					virtual bool IsIntersectsWith(const CBaseFeatureCollection* pValue) const;

					/// пересечение множеств
					virtual bool IsIntersectsOrEmpty(const CBaseFeatureCollection* pValue) const;

					///вернет строковое имя характеристики
					virtual const char* GetFeatureName() const = 0;
					///вернет строковое описание характеристики
					virtual const char* GetFeatureDescription() const = 0;

					///вернет кол-во характеристик, входящих в коллекцию
					unsigned int GetFeatureCount() const;
					///вернет характеристику из коллекции по порядковому номеру
					const CBaseFeature* GetFeature(unsigned int nFeature) const;
				CBaseFeature* GetFeature(unsigned int nFeature);

				//Описание хранимых данных. Печатная версия (с минимумом лишней информации)
				std::string GetPrintableDataDescription() const;


				protected:
					///список указателей на все характеристики, входящие в набор
					CBaseFeature** m_ppFeatures;
					CBaseFeature*** m_pppFeatures;
					///количество характеристик в наборе
					unsigned int m_FeatureCount;
					///
					int m_unusedFreeSpace;
					///тип набора (для контроля корректности обобщенных операций)
					TFeatureCollectionType m_FeatureCollectionType;

					///добавляет указатель на характеристику в список характеристик
					/**
						вызывается в конструкторах классов-наследников для всех характеристик - членов этих классов
						при необходимости код данной функции переписать для повышения скорости работы конструкторов классов наследников
					*/
					void AddFeatureToCollection(CBaseFeature* pFeature);

					///вернет подстроку строди ХМЛ, которая описывает значения данной характеристики
					void GetFeatureXMLRegion(const std::string& sXML, std::string& s) const;

					///вернет строку - заголовок характеристики
					void AppendFeatureHeader(std::string& s) const;
					///вернет строку - закрывающую характеристику
					void AppendFeatureEnd(std::string& s) const;
					///вернет строку - тело характеристики
					void AppendFeatureValues(std::string& s) const;
				};
			
			}
		}
	}
}