#ifndef FileSystem_H
#define FileSystem_H

#include <fstream>
#include <vector>
#include <functional>
#include <algorithm>
#include "StlAuxiliary.h"

/*
   Работа с файлами и файловой системой
*/

namespace SS { namespace Lib {

//Префикс UNICODE файла
extern const BYTE g_UnicodeFilePrefix[2];

///////////////////////////////////////////////////////////////////////////////
// Вспомогательные структуры
///////////////////////////////////////////////////////////////////////////////
struct TAutoFindClose
{
   HANDLE hFind;

   TAutoFindClose( HANDLE hFind = INVALID_HANDLE_VALUE ): hFind(hFind) {}
   ~TAutoFindClose() { if(hFind != INVALID_HANDLE_VALUE) APL_CHECK(FindClose(hFind)); }
};

struct TAutoFileClose
{
   HANDLE hFile;

   TAutoFileClose( HANDLE hFile = INVALID_HANDLE_VALUE ): hFile(hFile) {}
   ~TAutoFileClose() { if(hFile != INVALID_HANDLE_VALUE) APL_CHECK(CloseHandle(hFile)); }
};


///////////////////////////////////////////////////////////////////////////////
//Класс позволяет просматривать в каталоге и во всех вложенных каталогах (если 
//необходимо) список файлов
//При этом можно накладывать ограничения на расширения файла
///////////////////////////////////////////////////////////////////////////////
class CDirectoryFileList
{
   typedef TCHAR CharT;
   typedef std::basic_string<TCHAR> StringT;

   struct SDirItem //Информация о каталоге
   {
      HANDLE Handle;                      //хендл для FindNextFile    
      StringT::size_type DirNameSize;     //Длина строки с названием каталога в формате "<имя_каталога>\"
      WIN32_FIND_DATA FindData;           //Данные по текущему файлу

      SDirItem(): Handle(INVALID_HANDLE_VALUE), DirNameSize(0) 
	  {
		  memset(&FindData, 0, sizeof(WIN32_FIND_DATA));
	  }
   };

   typedef std::vector<SDirItem> TFindHandles;
   typedef std::vector<StringT> TExtensions;

private:
   StringT m_CurFile;                  //Текущий файл
   StringT m_CurDir;                   //Текущий каталог
   StringT m_Mask;                     //Маска файлов
   bool m_RrecurSearch;                //Искать файлы рекурсивно
   WIN32_FIND_DATA m_FindData;         //Данные по текущему файлу, используется только при возвращении во вне
   TExtensions  m_Extensions;          //Допустимые расширения файлов

   TFindHandles m_FindHandles;         //Список хендлов информации о вложенных каталогах

private:
   //Находит первый файл в каталоге m_CurDir, по маске m_Mask
   void FindFirst( SDirItem &DirItem )
   {
      DirItem.Handle = FindFirstFile( (m_CurDir + _T("*")).c_str(), &DirItem.FindData );
   }

   //Найти следующий файл в каталоке DirItem, если такого файла нет то закрывается поиск функцией 
   //FindClose и DirItem.Handle приравнивается к INVALID_HANDLE_VALUE
   void FindNextOrClose( SDirItem &DirItem )
   {
      APL_ASSERT( DirItem.Handle != INVALID_HANDLE_VALUE );

      if( FindNextFile(DirItem.Handle, &DirItem.FindData) == FALSE )
      {
         APL_CHECK( FindClose(DirItem.Handle) );
         DirItem.Handle = INVALID_HANDLE_VALUE;
      }
   }

public:  
   CDirectoryFileList(): m_RrecurSearch(true) 
   {
	   memset( &m_FindData, 0, sizeof(WIN32_FIND_DATA) );
   }
   template<class InputIterator>
      explicit CDirectoryFileList(const StringT &DirName, InputIterator First, InputIterator Last, bool Recursive = true)
   {  OpenDir(DirName, First, Last, Recursive); }

   explicit CDirectoryFileList(const StringT &DirName, bool Recursive = true)
   {  OpenDir(DirName, Recursive); }


   ~CDirectoryFileList() { Clear(); }

   //Инициировать поиск в DirName всех файлов с расширениями [First, Last), и если Recursive == true то и во всех вложеных
   //каталогах. Для поиска первого файла необходимо вызывать Next()
   template<class InputIterator>
   void OpenDir( const StringT &DirName, InputIterator First, InputIterator Last, bool Recursive = true )
   {
      Clear(); 

      //Сортируем список с расширениями для ускорения поиска
      m_Extensions.assign( First, Last );
      std::sort(m_Extensions.begin(), m_Extensions.end());

      m_CurDir = DirName;
      m_RrecurSearch = Recursive;

      if( m_CurDir[m_CurDir.size() - 1] != _T('\\') )
         m_CurDir +=  _T('\\');

      SDirItem DirItem;

      DirItem.DirNameSize = m_CurDir.size();
      FindFirst(DirItem);

      m_FindHandles.push_back( DirItem );
   }

   //Инициировать поиск в DirName всех файлов, и если Recursive == true то и во всех вложеных
   //каталогах. Для поиска первого файла необходимо вызывать Next()
   void OpenDir( const StringT &DirName, bool Recursive = true )
   {
      OpenDir( DirName, static_cast<const StringT *>(0), static_cast<const StringT *>(0), Recursive );
   }


   //Очистить все данные поиска
   void Clear();

   //Найти следущий (первый файл)
   bool Next();

   const StringT &GetDir() const { return m_CurDir; }
   const StringT &GetFile() const { return m_CurFile; }
   const WIN32_FIND_DATA &GetFindData() const { return m_FindData; }
};

///////////////////////////////////////////////////////////////////////////////
// Класс позволяет корректно читать строки из UNICODE или ANSI файлов, при 
// этом позволяет корректно опредилять тип файла.
// Класс заменяет последовательность \n\r на \n (а точнее удаляет \r)
// Какие символы являются разделителями а какие нет, опредиляется стратегией.
///////////////////////////////////////////////////////////////////////////////
class CUnicodeFileReader: public NonCopyable
{
   typedef wchar_t TChar;
   typedef BYTE TByte;

private:   
   static const size_t BuffSize = 4 * 1024;
   //static const TByte UnicodePref[]; //Префикс для UNICODE файла
private:
   HANDLE m_hFile;
   TChar m_Buff[BuffSize];
   std::vector<CHAR> m_AuxBuff; //Дополнительный буфер для чтения
   TChar *m_pBuffBegin, *m_pBuffEnd;
   bool m_IsUnicode; //Является ли файл файлом UNICODE
   bool m_IsEof;     //Был достигнут конец файла и прочитанные данные уже обработаны

private:
   
   //Скорректировать буфер, заменить символы перевода 
   //корретки и новой строки одим символом новой строки
   //А точнее удалить все символы \r
   void CorrectBuffer();
   
   //Заполнить буффер и настроить указатели
   void FillBuffer();

public:
   struct SNoDelimCharacter: public std::unary_function<TChar, bool>
   {
      bool operator()( TChar Char ) const
      {
         return false;
      }
   };

   struct SSpaceCharacter: public std::unary_function<TChar, bool>
   {
      bool operator()( TChar Char ) const
      {
         return iswspace(Char) != 0;
      }
   };

   struct SNewLineCharacter: public std::unary_function<TChar, bool>
   {
      bool operator()( TChar Char ) const
      {
         return Char == L'\n';
      }
   };

   struct SNotAlphaNumericCharacter: public std::unary_function<TChar, bool>
   {
      bool operator()( TChar Char ) const
      {
         return !IsCharAlphaNumericW(Char) != 0;
      }
   };

public:
   CUnicodeFileReader(): m_hFile(INVALID_HANDLE_VALUE), m_pBuffBegin(0), m_pBuffEnd(0), m_IsEof(false), m_IsUnicode(false) 
   {
	   memset(m_Buff, 0, sizeof(TChar) * BuffSize);
   } 

   //Автоматически вызывает Open
   explicit CUnicodeFileReader( LPCTSTR szFileName ): m_hFile(INVALID_HANDLE_VALUE), m_pBuffBegin(0), m_pBuffEnd(0), m_IsUnicode(false)
   {
      Open( szFileName );
   }

   //Закрыть файл
   void Close() 
   { 
      if( m_hFile != INVALID_HANDLE_VALUE )
      { 
         ::CloseHandle(m_hFile); 
         m_hFile = INVALID_HANDLE_VALUE; 
      } 

      m_pBuffBegin = m_pBuffEnd;
   } 

   //Автоматическое закрытие файла
   ~CUnicodeFileReader(){ Close(); }

   //Можно ли продолжать чтение файла
   bool Eof() const { return m_pBuffBegin == 0; }

   enum EOpenResult 
   { 
      EOROk,              //Файл удачно открыт, можно начинать чтение
      EORCantOpen,        //Ошибка при открытии файла
      //EORUndefinedFromat  //Файл открыт но имеет неверный формат
   };

   //Открыть новый файл (автоматически закрывается текущий открытый файл)
   EOpenResult Open( LPCTSTR szFileName );

   //Прочитать из файла строку которая отделена от других некоторыми символами
   //Пустые строки также учитываются т.е. группа разделителей не считается за один
   //Str - Указатель на массив размером минимум MaxLen
   //MaxLen - Размер буфера (включая место под '\0')
   //CurLen - Длина выходной строки
   //IsDelim - предикат возвращающий true если символ - разделитель
   //ВОЗВР: true - если удалось записать полное слово в Str
   template<class TCmp>
   bool Read( TChar *Str, size_t MaxLen, size_t &CurLen, TCmp IsDelim )
   {
      APL_ASSERT_PTR( Str );
      
      bool CurDelim;
      TChar *pBegWord;
      ptrdiff_t FindSize; 

      CurLen = 0;

      if( Eof() )
      {
         *Str = _T('\0');
         return false;
      }

      --MaxLen; //Мы должны учитывать '\0'

      for(;;) 
      {
         pBegWord = m_pBuffBegin;

         //Находим разделитель, если он есть, то мы нашли полное слово
         CurDelim = false;
         while( m_pBuffBegin != m_pBuffEnd && MaxLen > 0 && !(CurDelim = IsDelim(*m_pBuffBegin))  ){ ++m_pBuffBegin; --MaxLen; }

         //Копируем всё что нашли
         FindSize = m_pBuffBegin - pBegWord;

         //Возможно применить std::copy, и сделать Str итератором, 
         //но она использует в своей реализации memmove, что медленнее в общем случае чем memcpy
         std::memcpy( Str, pBegWord, FindSize * sizeof(*pBegWord) ); 
         CurLen += FindSize;
         Str += FindSize;

         if( CurDelim ) //Мы полностью прочитали необходимую строку
         {
            ++m_pBuffBegin; //Пропустим разделитель
            *Str = _T('\0');
           
            //Заполняем буфер, если необходимо
            if( m_pBuffBegin == m_pBuffEnd ) FillBuffer();

            return true;
         }

         //Заполняем буфер, если необходимо
         if( m_pBuffBegin == m_pBuffEnd ) FillBuffer();

         //Мы так и не нашли разделитель, но буффер уже кончился
         if( MaxLen == 0 ) 
         {
            *Str = _T('\0');
            return m_pBuffBegin == m_pBuffEnd; //Возможно кончился файл, тогда мы нашли полную строку
         }

         //Возможно кончился файл
         if( m_pBuffBegin == m_pBuffEnd )
         {
            *Str = _T('\0');
            return true;
         }
      }
   }

   //Прочитать из файла строку которая отделена от других некоторыми символами
   //Пустые строки также учитываются т.е. группа разделителей не считается за один
   //Str - Выходная строка
   //IsDelim - предикат возвращающий true если символ - разделитель
   //ВОЗВР: true - если удалось записать полное слово в Str
   template<class TCmp>
   bool Read( std::basic_string<TChar> &Str, TCmp IsDelim )
   {
      bool CurDelim;
      TChar *pBegWord;

      Str.clear();

      if( Eof() ) return false;

      for(;;) 
      {
         pBegWord = m_pBuffBegin;

         //Находим разделитель, если он есть, то мы нашли полное слово
         CurDelim = false;
         while( m_pBuffBegin != m_pBuffEnd && !(CurDelim = IsDelim(*m_pBuffBegin))  ){ ++m_pBuffBegin; }

         //Копируем всё что нашли         
         Str.append( pBegWord, m_pBuffBegin );

         if( CurDelim ) //Мы полностью прочитали необходимую строку
         {
            ++m_pBuffBegin; //Пропустим разделитель

            //Заполняем буфер, если необходимо
            if( m_pBuffBegin == m_pBuffEnd ) FillBuffer();

            return true;    
         }

         //Заполняем буфер, если необходимо
         if( m_pBuffBegin == m_pBuffEnd ) FillBuffer();

         //Возможно кончился файл
         if( m_pBuffBegin == m_pBuffEnd )
         {
            return true;
         }
      }
   }
};

///////////////////////////////////////////////////////////////////////////////
// Копирует все подкаталоги и файлы из одного каталога в другой
// From -  От куда копировать
// To -    Куда копировать
// Возвр - ERROR_SUCCESS - удачное завершение или значение от GetLastError в случие неудачи
///////////////////////////////////////////////////////////////////////////////
DWORD CopyDirectory( const std::basic_string<TCHAR> &From, const std::basic_string<TCHAR> &To );

///////////////////////////////////////////////////////////////////////////////
// Удаляет каталог и все вложенные файлы и каталоги
// Dir -  Удаляемый каталог
// Возвр - ERROR_SUCCESS - удачное завершение или значение от GetLastError в случие неудачи
///////////////////////////////////////////////////////////////////////////////
DWORD DeleteDirectory( const std::basic_string<TCHAR> &Dir );

///////////////////////////////////////////////////////////////////////////////
// Создаёт все каталоги по пути Dir, если они ещё не созданы. Т.е. после работы
// функции можно быть уверенным что в системе существует путь Dir (за исключением 
// возникновения каких либо системных ошибок)
// Возвр - ERROR_SUCCESS - удачное завершение или значение от GetLastError в случие неудачи
///////////////////////////////////////////////////////////////////////////////
DWORD CreateFullPath( const std::basic_string<TCHAR> &Dir );

///////////////////////////////////////////////////////////////////////////////
// Класс позволяет считывать из файла слова (последовательности из букв или цифр)
///////////////////////////////////////////////////////////////////////////////
class CFileWordReader
{
   std::ifstream m_flIn;
   std::string m_Buf;
   std::string::iterator m_Cur;

private:
   bool ValidChar( char C ) const
   {
      return IsCharAlphaNumericA(C) == TRUE;
   }

public:
   CFileWordReader(): m_Cur(m_Buf.end()) {}
   CFileWordReader( const char *FileName ): m_Cur(m_Buf.end()) { OpenFile(FileName); }

   bool OpenFile( const char *FileName )
   {
      m_flIn.close();
      m_flIn.clear();
      m_Cur = m_Buf.end();
      m_flIn.open(FileName);

      return m_flIn.good();
   }

   bool Get( std::string &Str );

   std::ifstream &GetStream() {return m_flIn;}
};
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// Очистить буфер входного потока
///////////////////////////////////////////////////////////////////////////////
template< class CharT, class CharTraitsT>
inline void ClearIStreamBuff( std::basic_istream<CharT, CharTraitsT> &IStream )
{
   IStream.clear();
   IStream.ignore(IStream.rdbuf()->in_avail());
}

///////////////////////////////////////////////////////////////////////////////
// Корректно записать в файл UNICODE строку
// OStream необходимо открывать в бинарном режиме
///////////////////////////////////////////////////////////////////////////////
inline std::ostream &WriteUnicode( std::ostream &OStream, std::wstring Str )
{
   return OStream.write( 
      reinterpret_cast<const char *>(Str.c_str()), 
      static_cast<std::streamsize>( sizeof(std::wstring::value_type) * Str.size() ) 
   );
}

inline std::ostream &WriteUnicode( std::ostream &OStream, const wchar_t *Str )
{
   return OStream.write( 
      reinterpret_cast<const char *>(Str), 
      static_cast<std::streamsize>( sizeof(*Str) * lstrlenW(Str) ) 
   );
}

///////////////////////////////////////////////////////////////////////////////
// Прочитать строку Str из UNICODE файла c именем FileName
// Возвр - ERROR_SUCCESS - удачное завершение или значение от GetLastError в случие неудачи
///////////////////////////////////////////////////////////////////////////////
DWORD ReadUnicodeFile( const std::basic_string<TCHAR> &FileName, std::basic_string<WCHAR> &Str );

///////////////////////////////////////////////////////////////////////////////
// Записать строку Str в UNICODE файл c именем FileName
// Возвр - ERROR_SUCCESS - удачное завершение или значение от GetLastError в случие неудачи
///////////////////////////////////////////////////////////////////////////////
DWORD WriteUnicodeFile( const std::basic_string<TCHAR> &FileName, const std::basic_string<WCHAR> &Str );

///////////////////////////////////////////////////////////////////////////////
// Записать строку Str в ANSI файл c именем FileName
// Возвр - ERROR_SUCCESS - удачное завершение или значение от GetLastError в случие неудачи
///////////////////////////////////////////////////////////////////////////////
DWORD WriteAnsiFile( const std::basic_string<TCHAR> &FileName, const std::basic_string<WCHAR> &Str );

///////////////////////////////////////////////////////////////////////////////
// Прочитать строку Str из UNICODE или ANSI файла (формат определяется автоматически) c именем FileName
// Возвр - ERROR_SUCCESS - удачное завершение или значение от GetLastError в случие неудачи
///////////////////////////////////////////////////////////////////////////////
DWORD ReadUnicodeAnsiFile( const std::basic_string<TCHAR> &FileName, std::basic_string<WCHAR> &Str );

///////////////////////////////////////////////////////////////////////////////
// Загрузить ресурс из некоторого файла объект или группу объектов типа T
//    hModule - Модуль в котором находятся ресурсы
//    ResourceType - тип ресурса 
//    ResourceID - Идентификатор ресурса 
//    pData - Указатель на данные. Освобождение данных после использования 
//       не требуется - об этом заботится Windows
//    Size - количество объектов типа T на которые указыавает Size
//    Возвр - ERROR_SUCCESS - удачное завершение или значение от GetLastError 
//       в случие неудачи. Если не удалось записать ни одного объека типа T, то
//       возвр ERROR_INVALID_DATA.
///////////////////////////////////////////////////////////////////////////////
template<class T>
DWORD LoadResource( HMODULE hModule, const TCHAR *ResourceType, WORD ResourceID, T *&pData, size_t &Size )
{
   APL_ASSERT( hModule );
   APL_ASSERT_PTR( ResourceType );

   HRSRC hResource;
   HGLOBAL hMemory;
   DWORD ResourceSize;

   hResource = ::FindResource( hModule, MAKEINTRESOURCE(ResourceID), ResourceType );
   if( !hResource ) return GetLastError();

   ResourceSize = ::SizeofResource( hModule, hResource ); 
   if( !ResourceSize ) return GetLastError();
   
   if( ResourceSize % sizeof(T) != 0 ) return ERROR_INVALID_DATA;

   Size = ResourceSize / sizeof(T);

   hMemory = ::LoadResource( hModule, hResource );
   if( !hMemory ) return GetLastError();

   pData  = (T *)::LockResource( hMemory );
   if( !pData ) return ERROR_INVALID_DATA;


   return ERROR_SUCCESS;
}


}} //namespace SS::Lib 

#endif