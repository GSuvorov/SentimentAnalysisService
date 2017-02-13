#ifndef Serialization_H
#define Serialization_H

#include <objbase.h>
#include <msxml.h>
#include <sstream>
#include "Atlbase.h"
#include "Auxiliary.h"


/*
   Набор классов которые позволяют используя XML строку сохранять и загружать данные набора вложенных
   классов и контейнеров.
   
   Для того чтобы воспользоваться Сохранением/Загрузкой несоставные элементы (такие как int, double, 
   std::string ) должны выполнять [Правило 1] 

   [Правило 1: Начало]
      Некоторый тип данных должен перегружать функции ReadFromStream, WriteToStream. 
      Реализация этих функций по-умолчанию вызвает опрераторы >> и << соответственно, 
      для заранее определённого потока.

      template< class ObjectT, class CharT, class CharTraitsT >
      void WriteToStream( std::basic_ostream<CharT, CharTraitsT> &Ostream, const ObjectT &Object );

      template< class ObjectT, class CharT, class CharTraitsT >
      void ReadFromStream( std::basic_istream<CharT, CharTraitsT> &Istream, ObjectT &Object );

      Если работа операторов >> и << удовлетворительна, то перегрузку делать не обязательно.
   [Правило 1: Конец]


   Для того чтобы воспользоваться Сохранением/Загрузкой каждый класс (или структура) в котором 
   находятся несколько вложенных переменных должен выполнять [Правило 2] 

   [Правило 2: Начало]
      Класс должен определять метод:       
      template< class Archive > void Serialize( Archive &A ) 

      либо перегрузить функцию 

      template< class Archive >void SS::Lib::Serialize ( Archive &A, T& ob )
      где T класс который неободимо читать/писать.

      Archive - класс который позволяет считывать/записывать как отдельные переменные, 
      так и вложенные классы и контейнеры, имеет след-ий интерфейс:*/
#if 0
      //Сохранить/Загрузить отдельный член ob структуры данных (class или struct) под именем Name
      //при этом T должен выполнять [Правило 1]
      void Element( const TChar *Name, /*const*/ T &ob );

      //Сохранить/Загрузить отдельный член структуры данных (class или struct) ob под именем Name
      //при этом T должен выполнять [Правило 1] 
      //Accessor - класс который позволяет получить доступ к члену структуры данных [Правило 3]
      template< class T, class AccessorT >
      void Element( const TChar *Name, /*const*/ T &ob, AccessorT Accessor );

      //Сохранить/Загрузить вложенную структуру ob структуры данных (class или struct) под именем Name
      //При этом T должен выполнять [Правило 2]
      template< class T >
      void Group( const TChar *Name, /*const*/ T &ob );

      //Сохранить/Загрузить вложенную структуру ob структуры данных (class или struct) под именем Name
      //При этом T должен выполнять [Правило 2] 
      //Accessor - класс который позволяет получить доступ к члену структуры данных [Правило 3]
      template< class T, class AccessorT >
      void Group( const TChar *Name, /*const*/ T &ob, AccessorT Accessor );

      //Сохранить/Загрузить STL контейнер ob структуры данных (class или struct) под именем Name.
      //ob содержит простые элементы для которых должно выполняться [Правило 1].
      //Каждый отдельный элемент сохраняется под именем ItemName
      template< class T >
      void STLContainerElement( const TChar *Name, const TChar *ItemName, /*const*/ T &ob );
      
      //Сохранить/Загрузить STL контейнер ob структуры данных (class или struct) под именем Name.
      //ob содержит простые элементы для которых должно выполняться [Правило 1].
      //Каждый отдельный элемент сохраняется под именем ItemName
      //Accessor - класс который позволяет получить доступ к котейнеру [Правило 3]
      template< class T, class AccessorT >
      void STLContainerElement( const TChar *Name, const TChar *ItemName, /*const*/ T &ob, AccessorT Accessor );

      //Сохранить/Загрузить STL контейнер ob структуры данных (class или struct) под именем Name.
      //ob содержит струтуры данных для которых должно выполянться [Правило 2].
      //Каждый отдельный элемент сохраняется под именем ItemName
      template< class T >
      void STLContainerGroup( const TChar *Name, const TChar *ItemName, /*const*/ T &ob );
      
      //Сохранить/Загрузить STL контейнер ob структуры данных (class или struct) под именем Name.
      //ob содержит струтуры данных для которых должно выполянться [Правило 2].
      //Каждый отдельный элемент сохраняется под именем ItemName
      //Accessor - класс который позволяет получить доступ к котейнеру [Правило 3]
      template< class T, class AccessorT >
      void STLContainerGroup( const TChar *Name, const TChar *ItemName, /*const*/ T &ob, AccessorT Accessor );
#endif
/*
   [Правило 2: Конец]

   [Правило 3: Начало]
      Интерфейс AccessorT
      Существует возможность абстрагироваться от конкретного вида получения 
      параметров при помощи аргумента Accessor в функциях класса CWriteArchive. При этом Accessor
      должен поддерживать один из след-иих интерфейсов.

      interface AccessorT
      {
         typedef ... ValueType; //Некоторый тип к которому открывается доступ

         //Взять у структуры данных занчение требуемого члена
         ValueType Get( Object &Obj ) const;  

         //Установить у структуры данных занчение требуемого члена
         void Set( Object &Obj, ValueType Val ) const;
      };

      interface AccessorT
      {
         typedef ... ValueType; //Некоторый тип к которому открывается доступ

         //Взять у структуры данных занчение требуемого члена
         const ValueType &Get( Object &Obj ) const;

         //Установить у структуры данных занчение требуемого члена
         void Set( Object &Obj, const ValueType & Val ) const;
      };
   [Правило 3: Конец]

   Для записи необходимо инстанцировать класс CWriteArchive. Затем вызвать метод: */
#if 0  
   //Сохранить все элементы структуры и все вложенные в неё структуры в поток
   //Name - Название корня структуры данных
   //ob - ссылка на структуру данных (class или struct)
   //При этом T должен выполнять [Правило 2]
   template< class T > void Save( const TChar *Name,  T &ob );
#endif
/*
   Для чтения необходимо инстанцировать класс CReadArchive. Затем вызвать метод: */
#if 0  
   //Загрузить все элементы структуры и все вложенные в неё структуры в поток
   //Name - Название корня структуры данных
   //ob - ссылка на структуру данных (class или struct)
   //При этом T должен выполнять [Правило 2]
   template< class T > void Load( const TChar *Name,  T &ob );
#endif


#if 0
//Простой пример (более сложный см. Test\Serialization\Main.cpp)
struct Foo
{
   int m_A;
   double m_B;
   std::basic_string<TCHAR> m_C;

   Foo(): m_A(1), m_B(-12.3e55), m_C(_T("Any String")) {}

   template< class Archive >
   void Serialize( Archive &A ) 
   {
      A.Element( _T("a"), m_A );
      A.Element( _T("b"), m_B );
      A.Element( _T("c"), m_C );
   }
};

struct Bar
{
   Foo F1;
   Foo F2;
   std::basic_string<TCHAR> m_Name;
   
   Bar(): m_Name(_T("Any Name")) {}

   template< class Archive >
   void Serialize( Archive &A ) 
   {
      A.Element( _T("name"), m_Name );
      A.Group( _T("foo1"), F1 );
      A.Group( _T("foo2"), F2 );
   }

   basic_string<TCHAR> GetXML()
   {
      basic_stringstream<TCHAR> Stream;
      Bar B;
      CWriteArchive<basic_stringstream<TCHAR> > Archive(Stream);
      Archive.Save( _T("bar"), B );
      return Stream.str();
   }
};

/* GetXML() возвращает:
<bar>
	<name>Any Name</name>
	<foo1>
		<a>1</a>
		<b>-1.23e+056</b>
		<c>Any String</c>
	</foo1>
	<foo2>
		<a>1</a>
		<b>-1.23e+056</b>
		<c>Any String</c>
	</foo2>
</bar>
*/

#endif

namespace SS { namespace Lib {

///////////////////////////////////////////////////////////////////////////////
// Функции обеспечивающие действия по-умолчанию, но позволяющие изменить его с 
// помощью перегрузки
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// Поведение по умолчанию для классов которые планируется читать-писать.
// Вызов метода Serialize. Можно перегружать для отдельных классов, а не писать
// функцию-член.
///////////////////////////////////////////////////////////////////////////////
template< class Archive, class T >
void Serialize( Archive &A, T& ob )
{
   ob.Serialize( A );
}

///////////////////////////////////////////////////////////////////////////////
// Запись в поток по-умолчанию. Вызов оператора <<
///////////////////////////////////////////////////////////////////////////////
template< class ObjectT, class CharT, class CharTraitsT >
void WriteToStream( std::basic_ostream<CharT, CharTraitsT> &Ostream, const ObjectT &Object )
{
   Ostream << Object;
}

///////////////////////////////////////////////////////////////////////////////
// Чтение из потока по-умолчанию. Вызов оператора >>
///////////////////////////////////////////////////////////////////////////////
template< class ObjectT, class CharT, class CharTraitsT >
void ReadFromStream( std::basic_istream<CharT, CharTraitsT> &Istream, ObjectT &Object )
{
   Istream >> Object;
}

template< class CharT, class CharTraitsT >
void ReadFromStream( std::basic_istream<CharT, CharTraitsT> &Istream, std::basic_string<CharT, CharTraitsT> &Object )
{
   std::getline( Istream, Object );
}


///////////////////////////////////////////////////////////////////////////////
// Классы позволяющие абстрагироваться от конкретного формата файла. 
// Необходимо только чтобы каждый формат поддерживал уровни вложенности информации и 
// позволял хранить даные на каждом уровне.
// Уровнем вложенности можно назвать приём аналогичный тому, как одна структура вложена 
// в другую. При этом можно считать то, что уровни не пересекаются, т.е. сначала обязательно 
// должен закончится более высокий уровень, а потом более низкий.
// Каждый класс реализует одни и те же интерфейсы. Затем от них наследуются 
// классы для непосредственной записи и чтения.
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
/*
//Интефейс для всех классов форматов файлов для записи
//Ostream - Класс который определяет оператор << для вывода переменных, либо
//          для которого перегружена функция WriteToStream
template < class CharT, class CharTraitsT = std::char_traits<CharT> >
interface Writer
{
protected:
   typedef std::basic_ostream< CharT, CharTraitsT > TOstream;
   typedef typename CharT TChar; 
   
   explicit Writer( TOstream &Os );

   //Начать новый увовень вложенности с именем Name
   void BeginLevel( const TChar *Name );
   
   //Закончить уровень вложенности с именем Name
   void EndLevel( const TChar *Name );
   
   //Сохранить Ob с именем Name
   //T должен выполнять [Правило 1]
   template< class T >
   void SaveItem( const TChar *Name, const T &Ob );

   //Начать/Закончить сохранение
   void BeginSave();
   void EndSave();
};
*/
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
/*
//Интефейс для всех классов форматов файлов для чтения
//Istream - Класс который определяет оператор >> для ввода переменных, либо
//          для которого перегружена функция ReadFromStream
template < class IstreamT >
interface Reader
{
protected:
   typedef std::basic_istream< CharT, CharTraitsT > TIstream;  
   typedef typename CharT TChar;  
   
   //Курсор, инкапсулирует внутреннюю реализацию поиска
   class TFindCursor{};
 
   explicit Reader( TIstream &Is );

   //Перейти на уровень Name
   void BeginLevel( const TChar *Name );
   
   //Выйти с уровня Name
   void EndLevel( const TChar *Name );
   
   //Загрузить Ob с именем Name
   //T должен выполнять [Правило 1]
   template< class T >
   void LoadItem( const TChar *Name, T &Ob );

   //Инициализировать поиск всех элементов с именем Name
   void FindInit( const TChar *Name, TFindCursor &FindCursor );
   
   //Найти следующий (в т.ч. первый) элемент и записать его значение в ob.
   //FindCursor - должен быть заранее инициализирован функцией FindInit
   //Возвр - true элемент успешно записан, false - элемент найти не удалось
   //T должен выполнять [Правило 1]
   template< class T >
   bool FindNext( TFindCursor &FindCursor, T &Ob);

   //Найти следующий (в т.ч. первый) элемент.
   //FindCursor - должен быть заранее инициализирован функцией FindInit
   //Возвр - true элемент успешно записан, false - элемент найти не удалось
   template< class T >
   bool FindNext( TFindCursor &FindCursor);

   //Начать/Закончить загрузку
   void BeginLoad();
   void EndLoad();
};
*/
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// Формат файла записи: Упрощённая запись в XML
// Не заменяет символы ( Например "<" на "&lt;" и д.р. )  
///////////////////////////////////////////////////////////////////////////////
template <  class CharT, class CharTraitsT = std::char_traits<CharT> >
class CSimpleXMLWriter
{
protected:
   typedef std::basic_ostream< CharT, CharTraitsT > TOstream;
   typedef typename CharT TChar;

private:
   TOstream &m_Os;

private:
   //Вывести в поток открывающийся тег
   void OpenTag( const TChar *Name )
   {
      //Как для Unicode так и для ANSI такой подход (static_cast<TChar>) будет работать
      m_Os << static_cast<TChar>('<') << Name << static_cast<TChar>('>');
   }
   
   //Вывести в поток закрывающийся тег
   void CloseTag( const TChar *Name )
   {
      //Как для Unicode так и для ANSI такой подход (static_cast<TChar>) будет работать
      m_Os << static_cast<TChar>('<') << static_cast<TChar>('/') << Name << static_cast<TChar>('>');
   }

protected:
   explicit CSimpleXMLWriter( TOstream &Os ): m_Os(Os) {}

   //Начать новый увовень вложенности с именем Name
   void BeginLevel( const TChar *Name )
   {
      OpenTag( Name );   
   }
   
   //Закончить уровень вложенности с именем Name
   void EndLevel( const TChar *Name )
   {
      CloseTag( Name );
   }

   //Сохранить данные Ob с именем Name
   //T должен выполнять [Правило 1]
   template< class T >
   void SaveItem( const TChar *Name, const T &Ob )
   {
      OpenTag( Name );   
      WriteToStream(m_Os, Ob);
      CloseTag( Name );
   }

   //Начать/Закончить загрузку
   void BeginSave() {}
   void EndSave() {}
};

///////////////////////////////////////////////////////////////////////////////
// Формат файла записи: Запись в XML при помощи MSXML
// Не заменяет символы ( Например "<" на "&lt;" и д.р. )  
///////////////////////////////////////////////////////////////////////////////

struct CCOMBase
{
   CCOMBase()
   {
      //Мне очень не нравится идея вызывать CoInitialize в конструкторе
      //и CoUninitialize в деструкторе, но это приходится делать т.к.
      //"это работает уже полгода без проблем"
      ::CoInitialize(NULL);
   }
   
   ~CCOMBase()
   {
      ::CoUninitialize();
   }

};
///////////////////////////////////////////////////////////////////////////////

//Запись в поток строки BSTR
template < class CharT, class CharTraitsT >
void WriteBSTRToStream( std::basic_ostream< CharT, CharTraitsT > &Ostream, BSTR Str )
{
   std::basic_string<CharT, CharTraitsT> StrTmp;
   Convert( std::wstring(Str), StrTmp );
   WriteToStream( Ostream, StrTmp );
}

template < class CharTraitsT >
void WriteBSTRToStream( std::basic_ostream< WCHAR, CharTraitsT > &Ostream, BSTR Str )
{
   //В этом случае никакие преобразования не нужны
   WriteToStream( Ostream, static_cast<const WCHAR *>(Str) );
}
///////////////////////////////////////////////////////////////////////////////

//Прочитать из потока строки BSTR
template < class CharT, class CharTraitsT >
void ReadBSTRFromStream( std::basic_istream< CharT, CharTraitsT > &Istream, CComBSTR &Str )
{ 
   Str.Empty();

   CharT Buff[1024];

   do 
   {
      Istream.read( Buff, APL_ARRSIZE(Buff) );
      CComBSTR TmpStr(Istream.gcount(), Buff);
      Str.Append(TmpStr);   

   } while(Istream.gcount() == APL_ARRSIZE(Buff));
}

template < class CharTraitsT >
void ReadBSTRFromStream( std::basic_istream< WCHAR, CharTraitsT > &Istream, CComBSTR &Str )
{ 
   //Оптимизация исключающая временные объекты
   Str.Empty();

   WCHAR Buff[1024];

   do 
   {
      Istream.read( Buff, APL_ARRSIZE(Buff) );
      Str.Append(Buff, Istream.gcount());   

   }
   while(Istream.gcount() == APL_ARRSIZE(Buff));

}


//Преобразовывает константную ссылку в неконстантную
template< class T >
static T &RemoveConst( const T & Obj )
{
   //Конечно не красиво использовать const_cast, но иначе сделать не получится т.к. 
   //метод Serialize принимает неконстантную ссылку
   return const_cast<T &>(Obj);
}

///////////////////////////////////////////////////////////////////////////////
// Запись в XML при помощи MSXML
///////////////////////////////////////////////////////////////////////////////
template < class CharT, class CharTraitsT = std::char_traits<CharT> >
class CXMLWriter: public CCOMBase
{
   typedef CComPtr<IXMLDOMDocument> TDOMDocument;
   typedef CComPtr<IXMLDOMElement> TDOMElement;
   typedef CComPtr<IXMLDOMNode> TDOMNode;
   typedef CComPtr<IXMLDOMText> TDOMText; 

protected:
   typedef std::basic_ostream< CharT, CharTraitsT > TOstream;
   typedef std::basic_ostringstream< CharT, CharTraitsT > TStringStream;
   typedef typename CharT TChar;

private:
   TOstream &m_Os;
   TDOMDocument m_DOMDocument;
   TDOMNode m_CurRootNode;
    
protected:
   explicit CXMLWriter( TOstream &Os ): m_Os(Os) {}

   //Начать/Закончить загрузку
   void BeginSave() 
   {
      m_DOMDocument.Release();
      m_CurRootNode.Release();

      if( m_DOMDocument.CoCreateInstance(__uuidof(DOMDocument)) != S_OK  )
         APL_THROW( _T("Ошибка при создании DOMDocument") );

      APL_CHECK_S_OK( m_DOMDocument.QueryInterface(&m_CurRootNode) );
   }

   void EndSave() 
   {
      CComBSTR Xml;
      BSTR TmpStr = 0;
      
      //Если использовать &Xml как параметр то BoundsChecker 7.2, необоснованно ругается на утечку памяти
      m_DOMDocument->get_xml( &TmpStr ); 
      Xml.Attach(TmpStr);
      
      WriteBSTRToStream( m_Os, Xml );
   }


   //Начать новый уровень вложенности с именем Name
   void BeginLevel( const TChar *Name )
   {
      TDOMElement Element;
      CComBSTR tagName( Name );
      TDOMNode NewChild;

      APL_CHECK_S_OK( m_DOMDocument->createElement(tagName, &Element) );
      APL_CHECK_S_OK( m_CurRootNode->appendChild(Element, &NewChild) );

      m_CurRootNode = NewChild;
   }

   //Закончить уровень вложенности с именем Name
   void EndLevel( const TChar *Name )
   {
      TDOMNode NewChild;
      
      APL_CHECK_S_OK( m_CurRootNode->get_parentNode(&NewChild) );
      m_CurRootNode = NewChild;
   }

   //Сохранить данные Ob с именем Name
   //Дожен быть определён оператор Ostream << Ob
   template< class T >
   void SaveItem( const TChar *Name, const T &Ob )
   {
      TStringStream StringStream;
      StringStream.copyfmt( m_Os );    //Настраиваем флаги

      WriteToStream( StringStream, Ob );
      
      CComBSTR Value( StringStream.str().c_str() );
      TDOMText TextNode;
      CComBSTR tagName( Name );
      TDOMNode NewChild;

      BeginLevel( Name );
      APL_CHECK_S_OK( m_DOMDocument->createTextNode(Value, &TextNode) );
      APL_CHECK_S_OK( m_CurRootNode->appendChild(TextNode, &NewChild) );
      EndLevel( Name );
   }
};


///////////////////////////////////////////////////////////////////////////////
// Чтение из XML при помощи MSXMl
///////////////////////////////////////////////////////////////////////////////
template < class CharT, class CharTraitsT = std::char_traits<CharT> >
class CXMLReader: public CCOMBase
{
   typedef CComPtr<IXMLDOMDocument> TDOMDocument;
   typedef CComPtr<IXMLDOMElement> TDOMElement;
   typedef CComPtr<IXMLDOMNode> TDOMNode;
   typedef CComPtr<IXMLDOMText> TDOMText; 
   typedef CComPtr<IXMLDOMNodeList> TDOMNodeList; 

protected:
   typedef std::basic_istream< CharT, CharTraitsT > TIstream;
   typedef std::basic_stringstream< CharT, CharTraitsT > TStringStream;
   typedef typename CharT TChar;

private:
   TIstream &m_Is;
   TDOMDocument m_DOMDocument;
   TDOMNode m_CurRootNode;


private:
   //Найти у узла ParentNode потомка с именем Name и присвоить ChildNode указатель на него
   //Возвр: Удалось ли найти
   bool FindChild( TDOMNode ParentNode, const CComBSTR &Name, TDOMNode &ChildNode )
   {
      TDOMNodeList NodeList;
      TDOMNode CurNode;
      CComBSTR NodeName;

      APL_CHECK_S_OK( ParentNode->get_childNodes(&NodeList) );

      long Length, i;

      APL_CHECK_S_OK( NodeList->get_length(&Length) );

      for( i = 0; i < Length; ++i )
      {
         CurNode.Release();
         NodeName.Empty();
         APL_CHECK_S_OK( NodeList->get_item(i, &CurNode) );
         APL_CHECK_S_OK( CurNode->get_nodeName(&NodeName) );

         if( Name == NodeName )
            break;
      }

      if( i != Length )
      {
         ChildNode = CurNode;

         return true;
      }

      return false;
   }

   //Прочитать текст узла 
   //T должен выполнять [Правило 1]
   template< class T >
   void ReadNode( TDOMNode Node, T &Ob )
   {
      TDOMNode TextNode;
      DOMNodeType NodeType;
      CComVariant Value;
      TDOMNodeList NodeList;
      long Length, i;

      APL_CHECK_S_OK( Node->get_childNodes(&NodeList) );
      APL_CHECK_S_OK( NodeList->get_length(&Length) );

      for( i = 0; i < Length; ++i )
      {
         TextNode.Release();
         APL_CHECK_S_OK( NodeList->get_item(i, &TextNode) );
         APL_CHECK_S_OK( TextNode->get_nodeType(&NodeType) );

         if( NodeType == NODE_TEXT )
            break;
      }

      TStringStream Stream;
      Stream.copyfmt(m_Is);   //Настраиваем флаги

      if( i < Length )
      {
         //При записи пустой строки текстовый потомок не создаётся
         //APL_THROW( _T("У элемента не найден ни один текстовый потомок") );

         APL_CHECK_S_OK( TextNode->get_nodeValue(&Value) );
         APL_ASSERT( Value.vt == VT_BSTR );

         WriteBSTRToStream( Stream, Value.bstrVal );
      }

      ReadFromStream( Stream, Ob );
   }

protected:
   //Курсор, инкапсулирует внутреннюю реализацию поиска
   struct TFindCursor
   {
   private:
      CComBSTR ElementName;   //Имя искомого элемента
      TDOMNodeList NodeList;  //Текущий список Узлов
      TDOMNode RootNode;      //Узел, который был текущем корнем до начала поиска

      friend class CXMLReader;   
   };

   explicit CXMLReader( TIstream &Is ): m_Is(Is) {}

   //Начать/Закончить загрузку
   void BeginLoad()
   {
      m_DOMDocument.Release();
      m_CurRootNode.Release();

      if( m_DOMDocument.CoCreateInstance(__uuidof(DOMDocument)) != S_OK  )
         APL_THROW( _T("Ошибка при создании DOMDocument") );
      
      APL_CHECK_S_OK( m_DOMDocument->put_async( FALSE ) );
      APL_CHECK_S_OK( m_DOMDocument->put_resolveExternals( FALSE ) );
      APL_CHECK_S_OK( m_DOMDocument.QueryInterface(&m_CurRootNode) );
      
      CComBSTR Str;
      VARIANT_BOOL isSuccessful;
      ReadBSTRFromStream( m_Is, Str );

      if( m_DOMDocument->loadXML( Str, &isSuccessful ) != S_OK || !isSuccessful/*isSuccessful != TRUE */) // isSuccessful почему-то равен -1
         APL_THROW( _T("Ошибка при загрузке XML") );
   }

   void EndLoad(){}

   //Перейти на уровень Name
   void BeginLevel( const TChar *Name )
   {
      TDOMNode CurNode;

      if( !FindChild(m_CurRootNode, Name, CurNode) )
         APL_THROW( _T("Не найден элемент с именем ") << Name );

      m_CurRootNode = CurNode;
   }

   //Выйти с уровня Name
   void EndLevel( const TChar *Name )
   {
      TDOMNode NewChild;

      APL_CHECK_S_OK( m_CurRootNode->get_parentNode(&NewChild) );
      m_CurRootNode = NewChild;
   }

   //Загрузить Ob с именем Name
   //T должен выполнять [Правило 1]
   template< class T >
   void LoadItem( const TChar *Name, T &Ob )
   {
      TDOMNode CurNode;

      if( !FindChild(m_CurRootNode, Name, CurNode) )
         APL_THROW( _T("Не найден элемент с именем ") << Name );
      
      ReadNode(CurNode, Ob);
   }

   //Инициализировать поиск всех элементов с именем Name
   void FindInit( const TChar *Name, TFindCursor &FindCursor )
   {
      FindCursor.ElementName = Name;
      FindCursor.NodeList.Release();
      FindCursor.RootNode = m_CurRootNode;
      APL_CHECK_S_OK( m_CurRootNode->get_childNodes(&FindCursor.NodeList) );
   }

   //Найти следующий (в т.ч. первый) элемент.
   //FindCursor - должен быть заранее инициализирован функцией FindInit
   //Возвр - true элемент успешно записан, false - элемент найти не удалось
   //T должен выполнять [Правило 1]
   template< class T >
   bool FindNext( TFindCursor &FindCursor, T &Ob)
   {
      TDOMNode CurNode;
      CComBSTR NodeName;

      for( ;; )
      {
         CurNode.Release();
         if( FindCursor.NodeList->nextNode(&CurNode) != S_OK )
         {
            m_CurRootNode = FindCursor.RootNode;
            return false;
         }

         NodeName.Empty();
         APL_CHECK_S_OK( CurNode->get_nodeName(&NodeName) );

         if( NodeName == FindCursor.ElementName )
         {
            ReadNode(CurNode, Ob);
            return true;
         }
      }
   }

   bool FindNext( TFindCursor &FindCursor )
   {
      TDOMNode CurNode;
      CComBSTR NodeName;

      for( ;; )
      {
         CurNode.Release();
         if( FindCursor.NodeList->nextNode(&CurNode) != S_OK )
         {
            m_CurRootNode = FindCursor.RootNode;
            return false;
         }

         NodeName.Empty();
         APL_CHECK_S_OK( CurNode->get_nodeName(&NodeName) );

         if( NodeName == FindCursor.ElementName )
         {
            m_CurRootNode = CurNode;
            return true;
         }
      }
   }

};

///////////////////////////////////////////////////////////////////////////////
// Класс для записи параметров. 
// Writer - Класс реализующий формат записи
///////////////////////////////////////////////////////////////////////////////
template<class CharT, template<class, class> class WriterT = CXMLWriter, class CharTraits = std::char_traits<CharT> >
class CWriteArchive: public WriterT<CharT, CharTraits>
{
public:
   typedef WriterT<CharT, CharTraits> TWriter;
   typedef typename TWriter::TChar TChar;
   typedef typename TWriter::TOstream TOstream;

public:
   //Конструктор должен обязательно принимать ссылку на поток
   explicit CWriteArchive( TOstream &Os ): TWriter( Os ) {}
   
   //Сохранить все элементы структуры и все вложенные в неё структуры в поток
   //Name - Название корня структуры данных
   //ob - ссылка на структуру данных (class или struct)
   //При этом T должен выполнять [Правило 2]
   template< class T >
   void Save( const TChar *Name, /*const*/ T &ob )
   {
      TWriter::BeginSave();
      TWriter::BeginLevel( Name );
      Serialize( *this, ob );
      TWriter::EndLevel( Name );
      TWriter::EndSave();
   }

   //Сохранить отдельный член ob структуры данных (class или struct) под именем Name
   //при этом T должен выполнять [Правило 1]
   template< class T >
   void Element( const TChar *Name, /*const*/ T &ob )
   {
      TWriter::SaveItem( Name, ob );
   }

   //Сохранить отдельный член структуры данных (class или struct) ob под именем Name
   //при этом T должен выполнять [Правило 1] 
   //Accessor - класс который позволяет получить доступ к члену структуры данных [Правило 3]
   template< class T, class AccessorT >
   void Element( const TChar *Name, /*const*/ T &ob, AccessorT Accessor )
   {
      TWriter::SaveItem( Name, Accessor.Get(ob) );
   }

   //Сохранить вложенную структуру ob структуры данных (class или struct) под именем Name
   //При этом T должен выполнять [Правило 2]
   template< class T >
   void Group( const TChar *Name, /*const*/ T &ob )
   {
      TWriter::BeginLevel( Name );
      Serialize( *this, ob );
      TWriter::EndLevel( Name );
   }

   //Сохранить вложенную структуру ob структуры данных (class или struct) под именем Name
   //При этом T должен выполнять [Правило 2] 
   //Accessor - класс который позволяет получить доступ к члену структуры данных [Правило 3]
   template< class T, class AccessorT >
   void Group( const TChar *Name, /*const*/ T &ob, AccessorT Accessor )
   {
      TWriter::BeginLevel( Name );
      Serialize( *this, RemoveConst(Accessor.Get(ob)) );
      TWriter::EndLevel( Name );
   }

   //Сохранить STL контейнер ob структуры данных (class или struct) под именем Name.
   //ob содержит простые элементы для которых должно выполняться [Правило 1].
   //Каждый отдельный элемент сохраняется под именем ItemName
   template< class T >
   void STLContainerElement( const TChar *Name, const TChar *ItemName, /*const*/ T &ob )
   {
      TWriter::BeginLevel( Name );
      
      for( typename T::const_iterator I = ob.begin(); I != ob.end(); ++I )
         TWriter::SaveItem( ItemName, *I );

      TWriter::EndLevel( Name );
   }
   
   //Сохранить STL контейнер ob структуры данных (class или struct) под именем Name.
   //ob содержит простые элементы для которых должно выполняться [Правило 1].
   //Каждый отдельный элемент сохраняется под именем ItemName
   //Accessor - класс который позволяет получить доступ к котейнеру [Правило 3]
   template< class T, class AccessorT >
   void STLContainerElement( const TChar *Name, const TChar *ItemName, /*const*/ T &ob, AccessorT Accessor )
   {
      TWriter::BeginLevel( Name );
      
      for( typename AccessorT::ValueType::const_iterator I = Accessor.Get(ob).begin(); I != Accessor.Get(ob).end(); ++I )
         TWriter::SaveItem( ItemName, *I );

      TWriter::EndLevel( Name );
   }

   //Сохранить STL контейнер ob структуры данных (class или struct) под именем Name.
   //ob содержит струтуры данных для которых должно выполянться [Правило 2].
   //Каждый отдельный элемент сохраняется под именем ItemName
   template< class T >
   void STLContainerGroup( const TChar *Name, const TChar *ItemName, /*const*/ T &ob )
   {
      TWriter::BeginLevel( Name );

      for( typename T::iterator I = ob.begin(); I != ob.end(); ++I )
      {
         TWriter::BeginLevel( ItemName );
         Serialize( *this, *I );
         TWriter::EndLevel( ItemName );
      }

      TWriter::EndLevel( Name );
   }
   
   //Сохранить STL контейнер ob структуры данных (class или struct) под именем Name.
   //ob содержит струтуры данных для которых должно выполянться [Правило 2].
   //Каждый отдельный элемент сохраняется под именем ItemName
   //Accessor - класс который позволяет получить доступ к котейнеру [Правило 3]
   template< class T, class AccessorT >
   void STLContainerGroup( const TChar *Name, const TChar *ItemName, /*const*/ T &ob, AccessorT Accessor )
   {
      TWriter::BeginLevel( Name );

      for( typename AccessorT::ValueType::const_iterator I = Accessor.Get(ob).begin(); I != Accessor.Get(ob).end(); ++I )
      {
         TWriter::BeginLevel( ItemName );
         Serialize( *this, RemoveConst(*I) );
         TWriter::EndLevel( ItemName );
      }

      TWriter::EndLevel( Name );
   }
};

///////////////////////////////////////////////////////////////////////////////
// Класс для чтения параметров. 
// Reader - Класс реализующий формат чтения
///////////////////////////////////////////////////////////////////////////////
template<class CharT, template<class, class> class ReaderT = CXMLReader, class CharTraits = std::char_traits<CharT> >
class CReadArchive: public ReaderT<CharT, CharTraits>
{
public:
   typedef ReaderT<CharT, CharTraits> TReader;
   typedef typename TReader::TChar TChar;
   typedef typename TReader::TIstream TIstream;
   typedef typename TReader::TFindCursor TFindCursor;


public:
   //Конструктор должен обязательно принимать ссылку на поток
   CReadArchive( TIstream &Is ): TReader( Is ) {}
   
   //Загрузить все элементы структуры и все вложенные в неё структуры в поток
   //Name - Название корня структуры данных
   //ob - ссылка на структуру данных (class или struct)
   //При этом T должен выполнять [Правило 2]
   template< class T >
   void Load( const TChar *Name, T &ob )
   {
      TReader::BeginLoad();
      TReader::BeginLevel( Name );
      Serialize( *this, ob );
      TReader::EndLevel( Name );
      TReader::EndLoad();
   }

   //Загрузить отдельный член ob структуры данных (class или struct) под именем Name
   //при этом T должен выполнять [Правило 1]
   template< class T >
   void Element( const TChar *Name, T &ob )
   {
      TReader::LoadItem( Name, ob );
   }

   //Загрузить отдельный член структуры данных (class или struct) ob под именем Name
   //при этом T должен выполнять [Правило 1] 
   //Accessor - класс который позволяет получить доступ к члену структуры данных [Правило 3]
   template< class T, class AccessorT >
   void Element( const TChar *Name, /*const*/ T &ob, AccessorT Accessor )
   {
      typename AccessorT::ValueType TmpOb;
      TReader::LoadItem( Name, TmpOb );
      Accessor.Set(ob, TmpOb);
   }

   //Загрузить вложенную структуру ob структуры данных (class или struct) под именем Name
   //При этом T должен выполнять [Правило 2]
   template< class T >
   void Group( const TChar *Name, T &ob )
   {
      TReader::BeginLevel( Name );
      Serialize( *this, ob );
      TReader::EndLevel( Name );
   }

   //Загрузить вложенную структуру ob структуры данных (class или struct) под именем Name
   //При этом T должен выполнять [Правило 2] 
   //Accessor - класс который позволяет получить доступ к члену структуры данных [Правило 3]
   template< class T, class AccessorT >
   void Group( const TChar *Name, T &ob, AccessorT Accessor )
   {
      TReader::BeginLevel( Name );
      Serialize( *this, RemoveConst(Accessor.Get(ob)) );
      TReader::EndLevel( Name );
   }

   //Загрузить STL контейнер ob структуры данных (class или struct) под именем Name.
   //ob содержит простые элементы для которых должно выполняться [Правило 1].
   //Каждый отдельный элемент сохраняется под именем ItemName
   template< class T >
   void STLContainerElement( const TChar *Name, const TChar *ItemName, T &ob )
   {
      typename T::value_type TmpOb;
      TFindCursor FindCursor;

      ob.clear();

      TReader::BeginLevel( Name );

      TReader::FindInit( ItemName, FindCursor );

      while( TReader::FindNext(FindCursor, TmpOb) )
         ob.push_back( TmpOb );

      TReader::EndLevel( Name );
   }
   
   //Загрузить STL контейнер ob структуры данных (class или struct) под именем Name.
   //ob содержит простые элементы для которых должно выполняться [Правило 1].
   //Каждый отдельный элемент сохраняется под именем ItemName
   //Accessor - класс который позволяет получить доступ к котейнеру [Правило 3]
   template< class T, class AccessorT >
   void STLContainerElement( const TChar *Name, const TChar *ItemName, T &ob, AccessorT Accessor )
   {
      typename AccessorT::ValueType TmpContainer;
      typename AccessorT::ValueType::value_type TmpValue;

      TFindCursor FindCursor;

      TReader::BeginLevel( Name );

      TReader::FindInit( ItemName, FindCursor );

      while( TReader::FindNext(FindCursor, TmpValue) )
      {
         TmpContainer.push_back(TmpValue);
      }

      TReader::EndLevel( Name );

      Accessor.Set(ob, TmpContainer);
   }

   //Загрузить STL контейнер ob структуры данных (class или struct) под именем Name.
   //ob содержит струтуры данных для которых должно выполянться [Правило 2].
   //Каждый отдельный элемент сохраняется под именем ItemName
   template< class T >
   void STLContainerGroup( const TChar *Name, const TChar *ItemName, T &ob )
   {
      typename T::value_type TmpOb;
      TFindCursor FindCursor;

      ob.clear();

      TReader::BeginLevel( Name );

      TReader::FindInit( ItemName, FindCursor );

      while( TReader::FindNext(FindCursor) )
      {
         Serialize( *this, TmpOb );
         ob.push_back( TmpOb );
      }

      TReader::EndLevel( Name );
   }
   
   //Загрузить STL контейнер ob структуры данных (class или struct) под именем Name.
   //ob содержит струтуры данных для которых должно выполянться [Правило 2].
   //Каждый отдельный элемент сохраняется под именем ItemName
   //Accessor - класс который позволяет получить доступ к котейнеру [Правило 3]
   template< class T, class AccessorT >
   void STLContainerGroup( const TChar *Name, const TChar *ItemName,  T &ob, AccessorT Accessor )
   {
      typename AccessorT::ValueType TmpContainer;
      typename AccessorT::ValueType::value_type TmpValue;

      TFindCursor FindCursor;

      TReader::BeginLevel( Name );

      TReader::FindInit( ItemName, FindCursor );

      while( TReader::FindNext(FindCursor) )
      {
         Serialize( *this, TmpValue );
         TmpContainer.push_back(TmpValue);
      }

      TReader::EndLevel( Name );

      Accessor.Set(ob, TmpContainer);
   }
};



///////////////////////////////////////////////////////////////////////////////
// Реализация интерфейса AccessorT [Правило 3] при помощи указателей на функции Set/Get
// Для использования необходимо вызвать функцию 
//    SetGetBind</*Тип структуры данных*/, /*Тип получаемого устанавлемого члена*/>(/*Указатель на функцию член Set*/, /*Указатель на функцию член Get*/)
//    Оба указатели на функции члены могут быть 0;
///////////////////////////////////////////////////////////////////////////////
template< class Object, class T >
struct CSetGetBindImpl
{
   typedef T ValueType;
   typedef void (Object::*SetFunc)(const T &);
   typedef const T &(Object::*GetFunc)() const;
   
   CSetGetBindImpl( SetFunc SF, GetFunc GF ): m_SetFunc(SF), m_GetFunc(GF) {}
   const T &Get( Object &Obj ) const { return (Obj.*m_GetFunc)(); }
   void Set( Object &Obj, const T &Val ) const { (Obj.*m_SetFunc)(Val); }

private:
   SetFunc m_SetFunc;
   GetFunc m_GetFunc;
};

template< class Object, class T >
struct CSetGetBindImplNoRef
{
   typedef T ValueType;
   typedef void (Object::*SetFunc)( T );
   typedef T (Object::*GetFunc)() const;

   CSetGetBindImplNoRef( SetFunc SF, GetFunc GF ): m_SetFunc(SF), m_GetFunc(GF) {}
   const T Get( Object &Obj ) const { return (Obj.*m_GetFunc)(); }
   void Set( Object &Obj, const T Val ) const { (Obj.*m_SetFunc)(Val); }

private:
   SetFunc m_SetFunc;
   GetFunc m_GetFunc;
};


template< class Object, class T >
CSetGetBindImpl<Object, T> SetGetBind(void (Object::*SetFunc)(const T &), const T &(Object::*GetFunc)() const )
{
   return CSetGetBindImpl<Object, T>( SetFunc, GetFunc);
}

template< class Object, class T >
CSetGetBindImplNoRef<Object, T> SetGetBind(void (Object::*SetFunc)(T), T (Object::*GetFunc)() const )
{
   return CSetGetBindImplNoRef<Object, T>(SetFunc, GetFunc);
}


/**
   Дополнительные функции облегчающие работу
*/

//Получить по классу Object в котором определен метод Serialize XML строку OutXML с 
//названием корневого тега RootName
template< class Object, class CharT >
void GetSerializeXMLString( Object &Object, const CharT *RootName, std::basic_string<CharT> &OutXML )
{
   std::basic_stringstream<CharT> Stream;
   CWriteArchive<CharT> Archive(Stream);
   Archive.Save( RootName, Object );
   OutXML = Stream.str();
}

//Прочитать из XML строки, с корневым тегом RootName, объект Object в котором определен метод Serialize
template< class Object, class CharT >
void SetSerializeXMLString( Object &Object, const CharT *RootName, const std::basic_string<CharT> &InXML )
{
   std::basic_stringstream<CharT> Stream(InXML);
   CReadArchive<CharT> Archive(Stream);
   Archive.Load( RootName, Object );
}
}} //namespace SS::Lib 

#endif