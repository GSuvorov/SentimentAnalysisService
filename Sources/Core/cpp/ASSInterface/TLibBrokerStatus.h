#pragma once

//##ModelId=3D479277033D

//#import "../../../libraries/msxml4.0/msxml4.dll"

//тип учета многозначности при поиске
enum PolySemyType{
	psPolySemy=0,	  //Вопрос с многозначностью
	psNoPolySemy=5,	//Вопрос без многозначности
};

//тип учета синтаксиса при поиске
enum SyntacsisType{
	stSyntacsis,	  //Поиск c синтаксическим разбором
	stNoSyntacsis,	//поиск без синтаксическим разбором
};


//тип учета близости и расположения при поиске
enum ENearness{
	enNearness,	  //Поиск c разбором по близости
	enNoNearness,	//поиск без разбора по близости
};

//тип учета частоты при поиске
enum EFrequency{
	efFrequency,	  //Поиск c анализом частоты встречаемости слов
	efNoFrequency,	//поиск без анализа частоты встречаемости слов
};

//тип учета отсутствия слова при поиске
enum AbsentType{
	abAbsent=0,	  //Проверка на присутствие слова в базе
	abNoAbsent=1,	//Нет проверки на присутствие слова в базе
};

//тип учета классов слов при поиске
enum ClassType{
	ctBase,	  //Поиск по основам
	ctOneRoot,//поиск по однокоренным словам
	ctClass,	//поиск по классам
	ctSynonymsOneRoot,
};


//тип учета семантики при поиске
enum EResponse{
	rpAll,  //Поиск всех ответов
	rpDirect, //Поиск прямых ответов
	rpIndirect, //поиск косвенных ответов
};

//описание режима поиска
struct TStatus{
	PolySemyType	m_PolySemy;
	SyntacsisType	m_Syntacsis;
	ENearness	m_Nearness;
	EFrequency	m_Frequency;
	ClassType	m_Class;
	AbsentType	m_Absent;
	EResponse	m_Response;
	bool	m_bExcludePredicate;

	void SetStatus(const TStatus& Value)
	{
		*this=Value;
	};

	void SetStatus(BSTR *pbstMode)
	{
		//CComBSTR bstXML;
		//CComVariant varValue;
		//CComBSTR bstClass=L"CLASS";
		//CComBSTR bstOneRoot=L"ONEROOT";
		//CComBSTR bstSynonymsOneRoot=L"SYNONYMSONEROOT";
		//CComBSTR bstBase=L"BASE";
		//CComBSTR bstSyntacsis=L"SYNTACSIS";
		//CComBSTR bstNoSyntacsis=L"NOSYNTACSIS";
		//CComBSTR bstPolySemy=L"POLYSEMY";
		//CComBSTR bstAbsent=L"ABSENT";
		//CComBSTR bstNearness=L"NEARNESS";
		//CComBSTR bstFrequency=L"FREQUENCY";

		//CComBSTR bstAll=L"ALL";
		//CComBSTR bstDirect=L"DIRECT";
		//CComBSTR bstIndirect=L"INDIRECT";

		//CComBSTR bstTRUE=L"1";

		//bstXML.Attach(*pbstMode);

		//MSXML2::IXMLDOMNodePtr oNdWord;
		//string sUndefinedWord;

		//MSXML2::IXMLDOMDocumentPtr m_oXMLDOMDoc;

		//HRESULT hr = m_oXMLDOMDoc.CreateInstance(__uuidof(MSXML2::DOMDocument40));

		//m_oXMLDOMDoc->loadXML(bstXML.Copy());


		///////////////////////////////////////////////////////////////////
		//oNdWord = m_oXMLDOMDoc->selectSingleNode(L"PROPERTIES/SEARCH");
		//if (oNdWord==NULL){
		//	//значение по умолчанию
		//	m_Class = ctBase;
		//}
		//else{
		//	oNdWord->get_nodeTypedValue(&varValue);
		//	if(bstClass == varValue.bstrVal){
		//		m_Class = ctClass;
		//	}
		//	else if(bstOneRoot == varValue.bstrVal){
		//		m_Class = ctOneRoot;
		//	}
		//	else if(bstSynonymsOneRoot == varValue.bstrVal){
		//		m_Class=ctSynonymsOneRoot;

		//	}
		//	else
		//		m_Class = ctBase;
		//}
		///////////////////////////////////////////////////////////////////

		///////////////////////////////////////////////////////////////////
		//oNdWord = m_oXMLDOMDoc->selectSingleNode(L"PROPERTIES/SYNTACSIS");
		//if (oNdWord==NULL){
		//	//значение по умолчанию
		//	m_Syntacsis = stNoSyntacsis;
		//}
		//else{
		//	oNdWord->get_nodeTypedValue(&varValue);
		//	m_Syntacsis = (bstSyntacsis==varValue.bstrVal) ? stSyntacsis : stNoSyntacsis ;
		//}
		///////////////////////////////////////////////////////////////////


		///////////////////////////////////////////////////////////////////
		////Нужна ли проверка на многозначность
		//oNdWord = m_oXMLDOMDoc->selectSingleNode(L"PROPERTIES/POLYSEMY");
		//if (oNdWord==NULL){
		//	//значение по умолчанию
		//	m_PolySemy = psNoPolySemy;
		//}
		//else{
		//	oNdWord->get_nodeTypedValue(&varValue);
		//	m_PolySemy = (bstPolySemy==varValue.bstrVal) ? psPolySemy : psNoPolySemy;
		//}
		///////////////////////////////////////////////////////////////////

		///////////////////////////////////////////////////////////////////
		////Нужна ли проверка на отсутствие слов
		//oNdWord = m_oXMLDOMDoc->selectSingleNode(L"PROPERTIES/ABSENT");
		//if (oNdWord==NULL){
		//	//значение по умолчанию
		//	m_Absent = abNoAbsent;
		//}
		//else{
		//	oNdWord->get_nodeTypedValue(&varValue);
		//	m_Absent = (bstAbsent==varValue.bstrVal) ? abAbsent : abNoAbsent;
		//}
		///////////////////////////////////////////////////////////////////

		///////////////////////////////////////////////////////////////////
		////Нужнен ли анализ на близость и расположение
		//oNdWord = m_oXMLDOMDoc->selectSingleNode(L"PROPERTIES/NEARNESS");
		//if (oNdWord==NULL){
		//	//значение по умолчанию
		//	m_Nearness = enNoNearness;
		//}
		//else{
		//	oNdWord->get_nodeTypedValue(&varValue);
		//	m_Nearness = (bstNearness == varValue.bstrVal) ? enNearness : enNoNearness;
		//}
		///////////////////////////////////////////////////////////////////

		///////////////////////////////////////////////////////////////////
		////Нужнен ли частотный анализ
		//oNdWord = m_oXMLDOMDoc->selectSingleNode(L"PROPERTIES/FREQUENCY");
		//if (oNdWord==NULL){
		//	//значение по умолчанию
		//	m_Frequency = efFrequency;
		//}
		//else{
		//	oNdWord->get_nodeTypedValue(&varValue);
		//	m_Frequency = (bstFrequency == varValue.bstrVal) ? efFrequency : efNoFrequency;
		//}
		///////////////////////////////////////////////////////////////////

		///////////////////////////////////////////////////////////////////
		////Нужно ли исключать из поиска сказуемые
		//oNdWord = m_oXMLDOMDoc->selectSingleNode(L"PROPERTIES/EXCLUDEPREDICATE");
		//if (oNdWord==NULL){
		//	//значение по умолчанию
		//	m_bExcludePredicate = false;
		//}
		//else{
		//	oNdWord->get_nodeTypedValue(&varValue);
		//	m_bExcludePredicate = (bstTRUE == varValue.bstrVal);
		//}
		///////////////////////////////////////////////////////////////////

		///////////////////////////////////////////////////////////////////
		////Нужнен ли частотный анализ
		//oNdWord = m_oXMLDOMDoc->selectSingleNode(L"PROPERTIES/RESPONSE");
		//if (oNdWord==NULL){
		//	//значение по умолчанию
		//	m_Response=rpAll;
		//}
		//else{
		//	oNdWord->get_nodeTypedValue(&varValue);
		//	if(bstAll==varValue.bstrVal)
		//		m_Response=rpAll;
		//	else if(bstDirect==varValue.bstrVal)
		//		m_Response=rpDirect;
		//	else if(bstIndirect==varValue.bstrVal)
		//		m_Response=rpIndirect;
		//}
		///////////////////////////////////////////////////////////////////
		//m_oXMLDOMDoc.Release();

		//bstXML.Detach();

	}

/*
  TStatus& operator = (const TStatus&  oStatus)
  {
    m_PolySemy=oStatus.m_PolySemy;
    m_Syntacsis=oStatus.m_Syntacsis;
    m_Nearness=oStatus.m_Nearness;
    m_Frequency=oStatus.m_Frequency;
    m_Class=oStatus.m_Class;
    m_Absent=oStatus.m_Absent;

    return *this;
  }
  */
};
