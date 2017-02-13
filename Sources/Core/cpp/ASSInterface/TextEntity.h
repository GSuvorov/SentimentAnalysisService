#pragma once

#include <string>
#include <vector>
#include <list>

namespace SS
{
	namespace Core
	{
		namespace Types
		{
			using std::wstring;
			using std::vector;
			using std::list;
			/// выделенная в тексте сущности
			class CTextEntity
			{
				public:
					CTextEntity()
						: m_value(L"")
						, m_entityType(L"Default")
						, m_startPosition(-1)
						, m_endPosition(-1)
						, m_mainNoun(L"")
						, m_mainNounPrefix(L"")
						, m_firstName(L"")
						, m_middleName(L"")
						, m_lastName(L"")
						, m_name(L"")
						, m_post(L"")
						, m_language(L"")
						, m_isPseudoPlace(false)
						, m_linkedPlaces(L"")
					{
					}

					CTextEntity(wstring value, wstring entityType, int startPosition, int endPosition, wstring mainNoun = L"", wstring mainNounPrefix = L"")
						: m_value(value)
						, m_entityType(entityType)
						, m_startPosition(startPosition)
						, m_endPosition(endPosition)
						, m_mainNoun(mainNoun)
						, m_mainNounPrefix(mainNounPrefix)
						, m_firstName(L"")
						, m_middleName(L"")
						, m_lastName(L"")
						, m_name(L"")
						, m_post(L"")
						, m_language(L"")
						, m_isPseudoPlace(false)
						, m_linkedPlaces(L"")
					{
					}

					CTextEntity(const CTextEntity &entity)
					{
						Copy(entity);
					}

					CTextEntity& operator=(const CTextEntity &entity)
					{
						Copy(entity);
						return *this;
					}

					virtual ~CTextEntity() {}
				
				private: /// методы
					/// Копирование значений входной сущности в текущую
					void Copy(const CTextEntity &entity)
					{
						m_value = entity.m_value;
						m_entityType = entity.m_entityType;
						m_startPosition = entity.m_startPosition;
						m_endPosition = entity.m_endPosition;
						m_mainNoun = entity.m_mainNoun;
						m_mainNounPrefix = entity.m_mainNounPrefix;
						m_morphoInformation = entity.m_morphoInformation;
						m_adjectives = entity.m_adjectives;
						m_firstName = entity.m_firstName;
						m_middleName = entity.m_middleName;
						m_lastName = entity.m_lastName;
						m_name = entity.m_name;
						m_post = entity.m_post;
						m_language = entity.m_language;
						m_isPseudoPlace = entity.m_isPseudoPlace;
						m_linkedPlaces = entity.m_linkedPlaces;
						m_subEntities = entity.m_subEntities;
					}

					/// добавление коллекции значений в указанную коллекцию
					/// source - коллекция значений
					/// result - обновляемая коллекция
					void add_Range(vector<wstring> source, vector<wstring> &result)
					{
						for (vector<wstring>::iterator it = source.begin(); it != source.end(); ++it)
							result.push_back(*it);
					}

				public: /// методы доступа к полям
					wstring get_Value() { return m_value; }
					void set_Value(wstring value) { m_value = value; }

					wstring get_EntityType() { return m_entityType; }
					void set_EntityType(wstring value) { m_entityType = value; }

					int get_StartPosition() { return m_startPosition; }
					void set_StartPosition(int value) { m_startPosition = value; }

					int get_EndPosition() { return m_endPosition; }
					void set_EndPosition(int value) { m_endPosition = value; }

					wstring get_MainNoun() { return m_mainNoun; }
					void set_MainNoun(wstring value) { m_mainNoun = value; }

					wstring get_MainNounPrefix() { return m_mainNounPrefix; }
					void set_MainNounPrefix(wstring value) { m_mainNounPrefix = value; }

					wstring get_FirstName() { return m_firstName; }
					void set_FirstName(wstring value) { m_firstName = value; }

					wstring get_MiddleName() { return m_middleName; }
					void set_MiddleName(wstring value) { m_middleName = value; }

					wstring get_LastName() { return m_lastName; }
					void set_LastName(wstring value) { m_lastName = value; }

					wstring get_Name() { return m_name; }
					void set_Name(wstring value) { m_name = value; }

					wstring get_Post() { return m_post; }
					void set_Post(wstring value) { m_post = value; }

					wstring get_Language() { return m_language; }
					void set_Language(wstring value) { m_language = value; }

					bool get_IsPseudoPlace() { return m_isPseudoPlace; }
					void set_IsPseudoPlace(bool value) { m_isPseudoPlace = value; }

					wstring get_LinkedPlaces() { return m_linkedPlaces; }
					void set_LinkedPlaces(wstring value) { m_linkedPlaces = value; }

					vector<wstring> get_MorphoInformation() { return m_morphoInformation; }
					void add_MorphoInformation(wstring value) { m_morphoInformation.push_back(value); }
					void add_MorphoInformation(vector<wstring> value) { add_Range(value, m_morphoInformation); }
					void clear_MorphoInformation() { m_morphoInformation.clear(); }

					vector<wstring> get_Adjectives() { return m_adjectives; }
					void add_Adjective(wstring value) { m_adjectives.push_back(value); }
					void add_Adjectives(vector<wstring> value) { add_Range(value, m_adjectives); }
					void clear_Adjectives() { m_adjectives.clear(); }

					vector<CTextEntity> get_SubEntities() { return m_subEntities; }
					void add_SubEntity(CTextEntity value) { m_subEntities.push_back(value); }
					void clear_SubEntities() { m_subEntities.clear(); }

				private: /// поля
					/// Нормализованное значение
					wstring m_value;
					/// Тип сущности
					wstring m_entityType;

					/// начальная позиция относительно начала текста
					int m_startPosition;
					/// конечная позиция относительно начала текста
					int m_endPosition;

					/// главное существительное цепочки
					wstring m_mainNoun;
					/// префикс главного существительного
					wstring m_mainNounPrefix;
					/// морфологическая информация
					vector<wstring> m_morphoInformation;
					/// все прилагательные цепочки
					vector<wstring> m_adjectives;

					/// имя
					wstring m_firstName;
					/// отчество
					wstring m_middleName;
					/// фамилия
					wstring m_lastName;
					/// название
					wstring m_name;
					/// должность
					wstring m_post;

					/// язык предложения
					wstring m_language;

					bool m_isPseudoPlace;
					wstring m_linkedPlaces;

					/// дочерние сущности, полученные правым отсечением
					/// только для ветки Роспатента
					vector<CTextEntity> m_subEntities;
			};
		}
	}
}