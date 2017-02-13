#pragma once

#include "../../ASCInterface/IInterface.h"
#include "../../ASCInterface/IDBMS.h"
#include "../../ASCInterface/defines.h"
#include "../../ASSInterface/ICommon.h"
#include "../../ASSInterface/TDBMS_Names.h"

USES_LOAD_MANAGER;

#include <string>
#include <vector>

namespace SS   {
	namespace Dictionary   {
		namespace Types   {

						/// Отражает информацию о части слова
			struct SPartOfWord   {

				/// Позиция в искомом слове
				INT16  PositionInWord;

				/// Идентификатор слова (IDSource)
				UINT32 Id;

				/// Конструктор
				SPartOfWord	() : PositionInWord(0), Id(0)	{}

			}; // struct SPartOfWord   {

			/// Отражает информацию о найденых гипотезах
			struct SHypothesisInfo   {

				/// Количество совпавших c искомым словом символов с конца
				UINT8  MatchSymbols;

				/// Идентификатор слова (IDSource)
				UINT32 Id;         

				/// Конструктор
				SHypothesisInfo	() : MatchSymbols(0), Id(0)	{}

			}; // struct SHypothesisInfo   {


		} // namespace Types   {



		namespace Core   {

			class IPartOfWordsSearch   {

				public:

					/// Инициализация путь к файлам данных и указатель на интерфейс таблицы IDataTable (для вызова Create())
					virtual BOOL Init(
						/// Путь к файлам данных
						CONST _TCHAR * pszPathToDataFiles,
						/// Указатель на интерфейс таблицы IDataTable (для вызова Create())
						SS::Interface::Core::DBMS::IDataTable * pIDataTable) = 0;

					/// Cоздает файлы, содержащие дерево и массив поиска
					virtual BOOL Create(VOID) = 0;

					/// Загружает/перезагружает словарь
					virtual BOOL Load(VOID) = 0;

					/// Освобождает память
					virtual BOOL Clear(VOID) = 0;

					/// Поиск в словаре всех возможных гипотез о слове, при этом гипотезой считается любое слово, у которого с искомым словом совпадают несколько символов с конца.
					virtual BOOL GetHypothesis(
						/// Входное слово для поиска
						IN CONST WCHAR * pwsPartOfWord,
						/// Позиция в слове, с которой необходимо начать поиск;  т.е. если необходимо найти слово “abc“ индекс будет 2, “ab” – 1 и т.д.
						IN INT iBeginPosition,
						/// Список выходных данных, перед вызовом необходимо вызвать reserve
						OUT std::vector< SS::Dictionary::Types::SHypothesisInfo > * p_v_HypothesisInfo,
						/// Минимальное количество совпадений, при котором слово записывается в <p_v_SHypothesisInfo>
						IN INT iMinMatchSymbols) = 0;

					/// Ищет все возможные пары частей слов и их индексов
					virtual BOOL GetByForm(
						/// Входное слово для поиска
						IN CONST WCHAR * pwsPartOfWord,
						/// Позиция в слове, с которой необходимо начать поиск
						IN INT iBeginPosition,
						/// Список выходных данных, перед вызовом необходимо вызвать reserve
						OUT std::vector< SS::Dictionary::Types::SPartOfWord > * p_v_PartOfWord,
						/// Критерий поиска: до начала слова или нет
						IN BOOL isCompleteWord = FALSE) = 0;

					/// Ищет часть слова по индексу
					virtual BOOL GetFormById(
						/// Индекс для поиска
						IN UINT iID, 
						/// Найденная часть слова
						OUT std::wstring * pwsPartOfWord) = 0;

			}; // class IPartOfWordsSearch

		} // namespace Core   {
	} // namespace Dictionary   {
} // namespace SS   {
