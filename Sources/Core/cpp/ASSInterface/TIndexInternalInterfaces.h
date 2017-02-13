//internal_interface.h 
namespace INDEX_INTERFACE
{


	//подготовка к чему-то
	class IPrepared
	{
	public:
		//Подготовка к считыванию информации
		virtual HRESULT Prepare()=0;

		virtual ~IPrepared() {};
	};

	//Возвращает ХБК для основы и уровня
	class IReturnSBC
	{
	public:

		//По четырем параметрам возвращает ХБК
		virtual HRESULT GetSBC(/*in*/ULONG ulNumLevel, /*in*/ULONG ulSourceIndex, /*in, out*/CSentenceLevel& oSentenceLevel) = 0; 

		virtual ~IReturnSBC() {};
	};

	//работа с базовой таблицей
	class IWorkWithBase
	{
	public:

		//Вставляет (апдейтит) данные по основе и возвращает номер последнего предложения, в котором эта основа встречалась
		//(0, если основа была вставлена)
		virtual HRESULT GetDataOfSource(ULONG ulSourceIndex, ULONG ulCurrentAbsNumOfSentence, ULONG &ulLastAbsNumOfSentence) = 0;

		virtual ~IWorkWithBase() {};
	};

	//работа с индексными таблицами
	class IWorkWithIndex
	{
	public:

		//Установка текущей основы
		virtual HRESULT SetSourceIndex(ULONG ulSourceIndex) = 0;

		//Установка текущего уровня
		virtual HRESULT SetNumLevel(ULONG ulNumLevel) = 0;

		//Увеличение текщего уровня на единицу
		virtual HRESULT IncLevel() = 0;

		//Вставка ХБК
		virtual HRESULT Insert(CSentenceLevel& oSentenceLevel, bool blIsInsert) = 0;

		virtual ~IWorkWithIndex() {};
	};

	//работа с таблицами соответствия
	class IWorkWithAccordance
	{
	public:

		//вставка данных
		virtual HRESULT Insert(ULONG ulAbsoluteNumSentence, ULONG ulNumText, ULONG ulNumParagraph, ULONG ulNumSentence) = 0;

		virtual ~IWorkWithAccordance() {};
	};

}
//internal_interface.h end




