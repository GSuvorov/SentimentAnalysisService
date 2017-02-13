#pragma once
#include "../ASCInterface/ICommonContainers.h"
#include "../ASDInterface/INdxQuery.h"
#include "./LingDecoder.h"
#include <iterator>
//------------------------------------------------------------------------------
typedef	std::set<unsigned char>				s_uchar;
typedef std::vector<unsigned int>			v_uint;
typedef std::vector<unsigned char>			v_uchar;

typedef	std::map< unsigned char, v_uint>	mv_uint;
typedef std::map< unsigned char, mv_uint>	mmv_uint;
typedef std::map< unsigned char, mmv_uint>	mmmv_uint;

typedef	std::set< unsigned char>			s_uchar;
typedef	std::map< unsigned char, s_uchar>	ms_uchar;
typedef std::map< unsigned char, ms_uchar>	mms_uchar;
typedef std::map< unsigned char, mms_uchar>	mmms_uchar;

typedef	std::set< unsigned int>				s_uint;
typedef	std::map< unsigned char, s_uint>	ms_uint;
typedef std::map< unsigned char, ms_uint>	mms_uint;
typedef std::map< unsigned char, mms_uint>	mmms_uint;


typedef std::map<unsigned char, bool>		m_bool;
typedef std::map<unsigned char, m_bool>		mm_bool;

typedef std::map<unsigned char, v_uchar>	mv_uchar;

typedef std::set<s_uint>					ss_uint;

typedef	std::vector< unsigned int>			v_uint;




//------------------------------------------------------------------------------
//	сокращение
typedef SS::Interface::Core::CommonContainers::IQueryIndex	IQI;

///	набор множеств индексов для слова
struct SWordIndexes
{
public:
	///	морфологические индексы слова
	s_uint	m_MorphoIndexes;
	///	морфологические индексы слова
	s_uint	m_SyntaxIndexes;
	///	морфологические индексы слова
	s_uint	m_SemanticIndexes;

	///	очистка
	__forceinline void	Clear()
	{
		m_MorphoIndexes.clear();
		m_SyntaxIndexes.clear();
		m_SemanticIndexes.clear();
	}


	///	признак пустоты
	__forceinline BOOL	Empty() const
	{
		return 
			(m_MorphoIndexes.empty() && 
			m_SyntaxIndexes.empty() &&
			m_SemanticIndexes.empty());
	}
};

///	набор векторов индексов для слова
struct SWordIndexes_vector
{
public:
	///	морфологические индексы слова
	v_uint	m_MorphoIndexes;
	///	морфологические индексы слова
	v_uint	m_SyntaxIndexes;
	///	морфологические индексы слова
	v_uint	m_SemanticIndexes;

	///	очистка
	__forceinline void	Clear()
	{
		m_MorphoIndexes.clear();
		m_SyntaxIndexes.clear();
		m_SemanticIndexes.clear();
	}

	///	присваивание
	__forceinline void assign(SWordIndexes const& _rhs)
	{
		Clear();
		std::copy(_rhs.m_MorphoIndexes.begin(), _rhs.m_MorphoIndexes.end(), back_inserter(m_MorphoIndexes));
		std::copy(_rhs.m_SyntaxIndexes.begin(), _rhs.m_SyntaxIndexes.end(), back_inserter(m_SyntaxIndexes));
		std::copy(_rhs.m_SemanticIndexes.begin(), _rhs.m_SemanticIndexes.end(), back_inserter(m_SemanticIndexes));
	}

	///	признак пустоты
	__forceinline BOOL	Empty()
	{
		return 
			(m_MorphoIndexes.empty() && 
			m_SyntaxIndexes.empty() &&
			m_SemanticIndexes.empty());
	}
};

//------------------------------------------------------------------------------
///	слово с флагами
struct flagged_word
{
public:
	SWordIndexes	m_Indexes;

	__forceinline flagged_word() :
	     m_IndexPosition(0)
		,m_WordsBlockID(0)
		,m_Obligatory(0)
		,m_bAnswerIndex(0)
		,m_LingWeight(0.0)
		,m_TransformationID(0)
		,m_SequenceID(0)
	{
	}

	//__forceinline flagged_word(s_uint const& _word)
	//	:m_IndexPosition(0), m_WordsBlockID(0), m_Obligatory(0), m_bAnswerIndex(0)
	//{
	//	m_MorphoIndexes.insert(_word.begin(), _word.end());
	//}

	//__forceinline flagged_word(
	//	SS::Interface::Core::CommonContainers::SQueryIndexAttributes const& _attribs)
	//	:m_IndexPosition(0), m_WordsBlockID(0), m_Obligatory(0), m_bAnswerIndex(0)
	//{
	//	set_TransformationID(_attribs.m_TransformationID);
	//	set_IndexPosition(_attribs.m_IndexPosition);
	//	set_UnionBlockID(_attribs.m_WordsBlockID);
	//	set_ObligatoryFlag(_attribs.m_Obligatory != 0);
	//	set_AnswerFlag(_attribs.m_bAnswerIndex != 0);
	//}

	//__forceinline flagged_word(
	//	s_uint const& _word,
	//	SS::Interface::Core::CommonContainers::SQueryIndexAttributes const& _attribs)
	//	:m_IndexPosition(0), m_WordsBlockID(0), m_Obligatory(0), m_bAnswerIndex(0)
	//{
	//	m_MorphoIndexes.insert(_word.begin(), _word.end());
	//	set_TransformationID(_attribs.m_TransformationID);
	//	set_IndexPosition(_attribs.m_IndexPosition);
	//	set_UnionBlockID(_attribs.m_WordsBlockID);
	//	set_ObligatoryFlag(_attribs.m_Obligatory != 0);
	//	set_AnswerFlag(_attribs.m_bAnswerIndex != 0);
	//}

	///	применение флагов из атрибутов индекса
	__forceinline void	ApplyIndexAttributes(SS::Interface::Core::CommonContainers::SQueryIndexAttributes const& _attribs)
	{
		set_TransformationID(_attribs.m_TransformationID);
		set_IndexPosition(_attribs.m_IndexPosition);
		set_UnionBlockID(_attribs.m_WordsBlockID);
		set_ObligatoryFlag(get_ObligatoryFlag() || (_attribs.m_Obligatory != 0));
		set_AnswerFlag(get_AnswerFlag() || (_attribs.m_bAnswerIndex != 0));
	}

	///	применение атрибутов слова (флагов, строкового представления) из такого же объекта
	///	добавляет строковое представление к существующему
	__forceinline void	ApplyWordAttributes(flagged_word const& _peer)
	{
		if (&_peer == this) return;
		add_AsString(_peer.get_AsString().c_str());
		apply_LingWeight(_peer.get_LingWeight());
		set_TransformationID(_peer.get_TransformationID());
		set_IndexPosition(_peer.get_IndexPosition());
		set_UnionBlockID(_peer.get_UnionBlockID());
		set_ObligatoryFlag(get_ObligatoryFlag() || _peer.get_ObligatoryFlag());
		set_AnswerFlag(get_AnswerFlag() || _peer.get_AnswerFlag());
	}

	///	Добавляет индексы другого слова
	__forceinline void	MergeWordIndexes(flagged_word const& _peer)
	{
		//	сливаем вместе индексы
		m_Indexes.m_MorphoIndexes.insert(_peer.m_Indexes.m_MorphoIndexes.begin(), _peer.m_Indexes.m_MorphoIndexes.end());
		m_Indexes.m_SyntaxIndexes.insert(_peer.m_Indexes.m_SyntaxIndexes.begin(), _peer.m_Indexes.m_SyntaxIndexes.end());
		m_Indexes.m_SemanticIndexes.insert(_peer.m_Indexes.m_SemanticIndexes.begin(), _peer.m_Indexes.m_SemanticIndexes.end());
	}


	///	добавление индекса в контейнер и обработка флагов из атрибутов индекса
	__forceinline void	AddApplyQueryIndex(IQI* _qi)
	{
		if (_qi == NULL) return;
		unsigned int	Index = _qi->GetQueryIndex();
		//	вставляем индексы в соответствии с их типом	\todo в зависимости от типа индекса добавлять его в нужный контейнер
		switch(LingDecoder::get_index_type(Index))
		{
			case LingDecoder::eitMorphoIndex:	m_Indexes.m_MorphoIndexes.insert(Index);	break;
			case LingDecoder::eitSyntaxIndex:	m_Indexes.m_SyntaxIndexes.insert(Index);	break;
			case LingDecoder::eitSemanticIndex:	m_Indexes.m_SemanticIndexes.insert(Index);	break;
			default: m_Indexes.m_MorphoIndexes.insert(Index);
		}
		//	добавляем строковое представление индекса
		add_AsString(_qi->GetQueryWord());
		//	добавляем лингвистический вес
		apply_LingWeight(_qi->GetIndexWeight());
		//	применяем атрибуты индекса
		ApplyIndexAttributes(_qi->GetAttributes());
	}

	///	устанавливает строковое представление
	__forceinline void set_AsString(const wchar_t* _cwszStr)
	{
		if (_cwszStr == NULL) return;
		m_StringWords.clear();
		m_StringWords.insert(std::wstring(_cwszStr));
	}

	///	добавляет строковое представление
	__forceinline void add_AsString(const wchar_t* _cwszStr)
	{
		if (_cwszStr == NULL) return;
		m_StringWords.insert(std::wstring(_cwszStr));
	}

	///	устанавливает лингвистический вес (установится только если он больше чем существующий)
	__forceinline void apply_LingWeight(float const& _weight){ if (_weight > m_LingWeight) m_LingWeight = _weight; }
	///	жёстко устанавливает лингвистический вес (не смотрит на существующий вес)
	__forceinline void set_LingWeight(float const& _weight){ m_LingWeight = _weight; }
	///	выдаёт лингвистический вес
	__forceinline float	get_LingWeight()const{ return m_LingWeight; }
	///	получает строковое представление
	__forceinline std::wstring get_AsString() const
	{
		//	если вообще не индексов, выдаём пустую строку
		if (m_StringWords.size() == 0) return std::wstring(L"");
		//	если одно слово то выдаём его
		if (m_StringWords.size() == 1) return *(m_StringWords.begin());
		//	если больше одного слова выдаём слепленную строку из всех слов
		wstring	temp;
		//	соединеям все слова в одну строку
		for(std::set< std::wstring >::const_iterator	it = m_StringWords.begin();
			it != m_StringWords.end();
			++it)
		{
			temp += (*it + std::wstring(L"!"));
		}
		return temp;
	}

	//	установка, получение флагов и параметров
	__forceinline void set_TransformationID(unsigned char const& _val){ m_TransformationID = _val;}
	__forceinline unsigned char get_TransformationID()const{ return m_TransformationID;}
	__forceinline void set_IndexPosition(unsigned char const& _val){ m_IndexPosition = _val;}
	__forceinline unsigned char get_IndexPosition()const{ return m_IndexPosition;}
	__forceinline void set_UnionBlockID(unsigned char const& _val){ m_WordsBlockID = _val;}
	__forceinline unsigned char get_UnionBlockID()const{ return m_WordsBlockID;}
	__forceinline void set_ObligatoryFlag(bool const& _val){ m_Obligatory = (_val)?(1):(0);}
	__forceinline bool get_ObligatoryFlag()const{ return (m_Obligatory != 0);}
	__forceinline void set_AnswerFlag(bool const& _val){ m_bAnswerIndex = (_val)?(1):(0);}
	__forceinline bool get_AnswerFlag()const{ return (m_bAnswerIndex != 0);}


	//	"оператор<" для слова
	bool	operator<(flagged_word const&	_rhs)const
	{
		//	наибольший приоритет
		bool b1 = m_Indexes.m_MorphoIndexes < _rhs.m_Indexes.m_MorphoIndexes;
		//	приоритет поменьше
		bool b2 = m_Indexes.m_SyntaxIndexes < _rhs.m_Indexes.m_SyntaxIndexes;
		//	приоритет ещё меньше
		bool b3 = m_Indexes.m_SemanticIndexes < _rhs.m_Indexes.m_SemanticIndexes;

		if (b1 || b2)
			return true;
		return b3;
	}

private:
	///	Строковое значение слова
	std::set< std::wstring >	m_StringWords;

	///	Флаги слова
	unsigned int
		///идентификатор трансформации индекса
		m_TransformationID : 8,
		///идентификатор последовательности индекса
		m_SequenceID : 8,
		///позиция индекса
		m_IndexPosition : 8,
		///идентификатор блока слов, 
		//если слова объеденены в блок, в результате достаточно присутствуие хотя бы N слов из блока
		m_WordsBlockID : 6,
		//в данный момент флаг обязательности
		m_Obligatory : 1,
		//флаг индексов ответа				
		m_bAnswerIndex : 1;

	///	Лингвистический вес слова
	float						m_LingWeight;
};


///	функтор для сравнения слов_с_флагами
struct flagged_word_less
{
public:
	bool operator()(flagged_word const& _lhs, flagged_word const& _rhs) const
	{
		return _lhs.operator<(_rhs);
	}
};

///	Обёртка для указателя, при создании инициализирует указатель в NULL.
///	В остальном ведёт себя как указатель.
///	Предназначена для использования в контейнерах map,
///	чтобы при обращении к элементу map'а элемент сам инициализировался в NULL
///	(в отличие от хранения в map'е обычного указателя)
template <typename _T>
class ptr_carrier
{
public:
	typedef _T	value_type;
	__forceinline ptr_carrier():ptr(NULL){}
	__forceinline void	operator=(value_type* _ptr){ptr = _ptr;}
	__forceinline const	value_type*	cget(){return ptr;}
	__forceinline value_type*	get(){return ptr;}
private:
	value_type*	ptr;
};

///	контейнеры с flagged_word
typedef std::set<flagged_word, flagged_word_less>	s_flagged_word;
typedef std::map<unsigned char, s_flagged_word>		ms_flagged_word;
typedef std::map<unsigned char, flagged_word>		m_flagged_word;
typedef std::map<unsigned char, m_flagged_word>		mm_flagged_word;
typedef std::map<unsigned char, mm_flagged_word>	mmm_flagged_word;
///	контейнеры с указателями на flagged_word
typedef std::map<unsigned char, ptr_carrier<flagged_word> >		m_pflagged_word;
typedef std::map<unsigned char, m_pflagged_word>	mm_pflagged_word;
typedef std::map<unsigned char, mm_pflagged_word>	mmm_pflagged_word;

////	вывод слова в отладку
//__forceinline void	ToConsole(flagged_word&	_fword)
//{
//	using	SS::Core::NdxSE::NdxQuery::ToConsole;
//	const size_t	sz = 512;
//	wchar_t	buf[sz];
//	size_t counter  = 0;
//	counter += _snwprintf(buf+counter, sz-counter, L"\ns_flagged_word: ");
//	for(s_uint::iterator itb_index = _fword.m_MorphoIndexes.begin(), ite_index = _fword.m_MorphoIndexes.end();
//		itb_index != ite_index;
//		++itb_index)
//		counter += _snwprintf(buf+counter, sz-counter, L"|%u", *itb_index);
//	ToConsole(buf);
//	counter = 0;
//	_snwprintf(
//		buf+counter,
//		sz-counter,
//		L"\n\t...flags: fO = %u, fAN = %u\n",
//		(unsigned int)_fword.get_ObligatoryFlag(),
//		(unsigned int)_fword.get_AnswerFlag());
//	ToConsole(buf);
//}
//
//__forceinline void ToConsole(s_flagged_word& _val)
//{
//	using	SS::Core::NdxSE::NdxQuery::ToConsole;
//	ToConsole(L"\ns_flagged_word:");
//	for(s_flagged_word::iterator	itb_word = _val.begin(), ite_word = _val.end();
//		itb_word != ite_word;
//		++itb_word)
//	{
//		ToConsole(*itb_word);
//	}
//}

//	вывод карты слов в отладку
//__forceinline void ToConsole(m_flagged_word& _val)
//{
//	using	SS::Core::NdxSE::NdxQuery::ToConsole;
//	ToConsole(L"\nm_flagged_word:");
//	for(m_flagged_word::iterator	itb_word = _val.begin(), ite_word = _val.end();
//		itb_word != ite_word;
//		++itb_word)
//	{
//		ToConsole(itb_word->second);
//		//ToConsole(L", transformation number: ", (unsigned int)itb_word->first);
//	}
//}


///	отлавливалка минимального значения
///	когда ей много раз присваивается значение (operator=), в ней остаётся минимальное из них
template <typename _T>
class min_catcher
{
public:
	//	запомнили тип
	typedef _T	value_type;
	//	конструктор, инициализация членов
	__forceinline	min_catcher():m_Exists(false), m_Val(0){}
	//	оператор присваивания, обрабатывает значение
	__forceinline	value_type operator=(value_type const& _val)
	{
		if (m_Exists)
		{
			if (_val < m_Val) m_Val = _val;
		}
		else
		{
			m_Val = _val;
			m_Exists = true;
		}
		return m_Val;
	}
	//	было ли хоть одно присваивание
	__forceinline	bool	exists()const{return m_Exists};
	//	получение минимального значения
	__forceinline	value_type	get()const{return m_Val;};

private:
	//	существование результата (было ли хоть одно присваивание)
	bool		m_Exists;
	//	минимальное значение
	value_type	m_Val;
};

///	контейнеры с min_catcher
typedef std::map<unsigned char, min_catcher<unsigned char> >	m_mcth_uc;
typedef std::map<unsigned char, m_mcth_uc>						mm_mcth_uc;
typedef std::map<unsigned char, mm_mcth_uc>						mmm_mcth_uc;

namespace SS
{
namespace Core
{
namespace NdxSE
{
namespace NdxQuery
{
//	контейнеры с IQueryIndex*
typedef	std::vector<IQI*>					v_IQI;
typedef	std::map< unsigned char, IQI*  >	m_IQI;
typedef	std::map< unsigned char, m_IQI >	mm_IQI;
typedef	std::map< unsigned char, v_IQI >	mv_IQI;
typedef	std::map< unsigned char, mv_IQI >	mmv_IQI;

typedef	std::map< unsigned char, const wchar_t*	>	m_cwsz;
typedef	std::map< unsigned char, m_cwsz			>	mm_cwsz;

//	сокращение
typedef SS::Interface::Core::NdxSE::SUnionWordsBlockInSequence UWB;
typedef std::vector<UWB>	v_UWB;
typedef std::map<unsigned char, vector<UWB>	>	mv_UWB;
typedef std::map<unsigned char, mv_UWB	>		mmv_UWB;

}
}
}
}