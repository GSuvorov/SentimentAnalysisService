#pragma once
#include "..\ASCInterface\IInterface.h"
#include "..\ASCInterface\ILinguisticProcessor.h"

// {81AFE03D-7E01-49af-B42D-745AB8C37133}
static const GUID	IID_ParametrizedQuery = 
{ 0x81afe03d, 0x7e01, 0x49af, { 0xb4, 0x2d, 0x74, 0x5a, 0xb8, 0xc3, 0x71, 0x33 } };

// {54AA4054-B51C-4efc-B66E-1CC2D7D35134}
static const GUID	CLSID_ParametrizedQuery = 
{ 0x54aa4054, 0xb51c, 0x4efc, { 0xb6, 0x6e, 0x1c, 0xc2, 0xd7, 0xd3, 0x51, 0x34 } };

namespace SS
{
	namespace Interface
	{
		namespace Core
		{
			namespace NdxSE
			{
				class ISuperTemplate;
				class IBlock;
				class IVariant;
				class IWord;
				class ISequence;

				/// Параметризированный запрос к индексному процессору
				/**
					Наследуется от IBase для обеспечения загрузки с помощью 
					модуля LoadManager
				*/
				class IParametrizedQuery
					: public virtual SS::Interface::IBase
				{
				public:
					/// инициализация запросом, сгенерированным лингвистическим процессором
					///	\param[in]	pQuery			Запрос
					///	\param[in]	pSearchParams	Параметры поиска
					virtual void Init(
						SS::Interface::Core::CommonContainers::IQueryResult*	pQuery,
						SS::Core::Types::IndexationSearch::TSearchParams*	pSearchParams) = 0;
					/// \return	Указатель на супертрансформацию
					virtual ISuperTemplate* GetSuperTemplate() = 0;

					/// \return	Количество вариантов
					virtual unsigned int GetVariantCount() = 0;
					///	\return	Вариант по номеру
					virtual IVariant* GetVariant(unsigned int uiIndex) = 0;

					///	\return Имеется ли хоть в одном индексе запроса флаг ответа
					virtual BOOL	GetHasAnswers() = 0;

					///	\return Имеются ли юнион-блоки во входных данных (IQueryResult)
					virtual BOOL	GetHasUnionBlocks() = 0;

					///	\return	Имеется ли хоть одно необязательное слово во входных данных (IQueryResult)
					///	(у всех индексов такого слова не установлены флаги обязательности)
					virtual BOOL	GetHasNonObligatoryWords() = 0;

					///	Выводит в виде строки своё содержимое
					///	\return	Возвращает указатель на внутреннее представление строки.
					virtual	const wchar_t*	ToString()=0;

					///	Выводит в виде строки необработанный запрос
					///	\return	Возвращает указатель на внутреннее представление строки.
					virtual	const wchar_t*	ToStringRawQuery(SS::Interface::Core::CommonContainers::IQueryResult* pQuery)=0;

					///	Отладочный вывод готового параметризированного запроса
					virtual void	DebugView() = 0;
					///	Отладочный вывод неоптимизированного запроса (входные данные)
					virtual void	DebugViewRawQuery(SS::Interface::Core::CommonContainers::IQueryResult* pQuery) = 0;
				};

				///	Запрос (супертрансформация)
				/**
				содержит в себе общий запрос для поиска индексным процессором:
					- "И"-блок слов. слова в этом блоке ищутся по "И", сам блок объединен с "ИЛИ"-блоками по "И"
					- набор "ИЛИ"-блоков. слова в каждом таком блоке ищутся по "И", сами "ИЛИ"-блоки объединены между собой по "ИЛИ"
				*/
				class ISuperTemplate
				{
				public:
					///	\return	Указатель на AND-блок
					virtual IBlock* GetANDBlock() = 0;
					///	\return Количество OR-блоков
					virtual unsigned int GetORBlockCount() = 0;
					///	\return	OR-блок по номеру
					virtual IBlock* GetORBlock(unsigned int uiIndex) = 0;
					///	Выводит в виде строки своё содержимое
					///	\return	Возвращает указатель на внутреннее представление строки.
					virtual	const wchar_t*	ToString()=0;
					///	Отладочный вывод объекта
					virtual void	DebugView() = 0;
				};

				/// Блок слов, входящий в запрос (супертрансформацию)
				/**
					Данный блок содержит набор слов (IWord)
					слова внутри блока ищутся по "И"
				*/
				class IBlock
				{
				public:
					///	\return	Количество слов в блоке
					virtual unsigned int GetWordCount() = 0;
					///	\return	Слово по номеру
					virtual IWord* GetWord(unsigned int uiIndex) = 0;
					///	Выводит в виде строки своё содержимое
					///	\return	Возвращает указатель на внутреннее представление строки.
					virtual	const wchar_t*	ToString()=0;
					///	Отладочный вывод объекта
					virtual void	DebugView() = 0;
				};

				/// Слово запроса
				/**
					Cлово запроса представляет собой наборы разных видов индексов, 
					которые располагаются на одной позиции,
					(все индексы в рамках одной позиции ищутся по "ИЛИ")
				*/
				class IWord
				{
				public:
					///	Получение количества морфологических индексов
					virtual unsigned int	GetMorphoIndexCount() = 0;
					///	Получение морфологического индекса по номеру
					virtual unsigned int	GetMorphoIndex(unsigned int uiIndex) = 0;
					///	Получение количества синтаксических индексов
					virtual unsigned int	GetSyntaxIndexCount() = 0;
					///	Получение синтаксического индекса по номеру
					virtual unsigned int	GetSyntaxIndex(unsigned int uiIndex) = 0;
					///	Получение количества семантических индексов
					virtual unsigned int	GetSemanticIndexCount() = 0;
					///	Получение семантического индекса по номеру
					virtual unsigned int	GetSemanticIndex(unsigned int uiIndex) = 0;

					///	\return Лингвистический вес слова
					virtual	float			GetLinguisticWeight() = 0;

					///	\return Флаг обязательности
					virtual BOOL			GetObligatoryFlag() = 0;
					///	\return	Флаг ответа
					virtual BOOL			GetAnswerFlag() = 0;

					///	\return	Текст слова (текст хранится в слове).
					///	никогда не NULL; если текста нет то указывает на ""
					///	Если у индексов из которых собиралось слово были разные 
					///	строковые представления, все они будут присутствовать в слове,
					///	перечисленные через "!"
					virtual const wchar_t*	GetAsString() = 0;

					///	Выводит в виде строки своё содержимое
					///	\return	Возвращает указатель на внутреннее представление строки.
					virtual	const wchar_t*	ToString()=0;

					///	Отладочный вывод объекта
					virtual void	DebugView() = 0;
				};

				/// "Вариант" запроса
				/**
				Вариант запроса представляет собой набор последовательностей 
				(ISequence) слов, которые могут располагаться в искомом тексте 
				в произвольном порядке; данные наборы слов ищутся по "И"
				*/
				class IVariant
				{
				public:
					///	\return	Число последовательностей
					virtual unsigned int GetSequenceCount() = 0;
					///	\return	Последовательность по номеру
					virtual ISequence* GetSequence(unsigned int uiIndex) = 0;
					///	Выводит в виде строки своё содержимое
					///	\return	Возвращает указатель на внутреннее представление строки.
					virtual	const wchar_t*	ToString()=0;
					///	Отладочный вывод объекта
					virtual void	DebugView() = 0;
				};


				///	Структура описывающая Union-блок
				///	в последовательности. Union-блок не может быть пустым
				///	т.к. uiFirstWordNumber и uiLastWordNumber указывают на
				///	начало и конец включительно
#pragma pack(push, 1)
				struct SUnionWordsBlockInSequence
				{
				public:
					///	Номер первого слова Union-блока (включительно)
					unsigned int	uiFirstWordNumber;
					///	Номер последнего слова Union-блока (включительно)
					unsigned int	uiLastWordNumber;
					///	Constructor
					SUnionWordsBlockInSequence(
						unsigned int uiFirst,
						unsigned int uiLast)
					:uiFirstWordNumber(uiFirst), uiLastWordNumber(uiLast)
					{}
				};
#pragma pack(pop)

				/// Последовательность слов
				/**
				Последовательность слов включает в себя слова, которые в искомом
				тексте должны идти в определенном порядке
				т.е. например слово2 после слова1, а слово1 после слова0
				при это данные слова ищутся по "И"
				Последовательность может содержать несколько Union-блоков слов
				начала и концы таких блоков содержатся в структурах SUnionWordsInSequence
				которые можно получать методами ISequence
				SUnionWordsBlockInSequence выдаются из ISequence в порядке следования
				идентификаторов union-блоков в последовательности
				*/
				class ISequence
					: public IBlock
				{
				public:
					///	\return	Количество Union-блоков
					virtual unsigned int GetUnionWordsBlocksCount() = 0;
					///	\return	Структуру описания union-блока по номеру
					virtual SUnionWordsBlockInSequence GetUnionWordsBlock(unsigned int uiIndex) = 0;
					///	Возвращает принадлежность слова какому-либо юнион-блоку с возвратом юнион-блока в случае успеха
					///	\param[in]	uiIndex	Номер слова (нумерация 0..(GetWordCount()-1))
					///	\param[out]	pUnion	Куда запишется информация о юнион-блоке в котором присутствует слово
					///	Если NULL, то ничего не запишется по указателю.
					///	\return	true - слово принадежит какому-либо юнион-блоку, по указателю pUnion запишется
					///	информация об этом блоке; false - слово не принадлежит никакому юнион блоку
					virtual BOOL	IsWordInUnion(unsigned int uiIndex, SUnionWordsBlockInSequence* pUnion) = 0;
					///	Выводит в виде строки своё содержимое
					///	\return	Возвращает указатель на внутреннее представление строки.
					virtual	const wchar_t*	ToString()=0;
					///	Отладочный вывод объекта
					virtual void	DebugView() = 0;
				};
			}
		}
	}
}