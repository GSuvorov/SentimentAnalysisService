#ifndef ProgressHPP
#define ProgressHPP
//Класс отображает ProgressBar в окне консоли (взят из boost.timer)
//С моими изменениями и исправлением ошибок

//  progress_display  --------------------------------------------------------//

//  progress_display displays an appropriate indication of 
//  progress at an appropriate place in an appropriate form.

// NOTE: (Jan 12, 2001) Tried to change unsigned long to boost::uintmax_t, but
// found some compilers couldn't handle the required conversion to double.
// Reverted to unsigned long until the compilers catch up. 

namespace SS { namespace Lib {

class progress_display : private NonCopyable
{
public:
   explicit progress_display( unsigned long expected_count,
      std::ostream & os = std::cout,
      const std::string & s1 = "\n", //leading strings
      const std::string & s2 = "",
      const std::string & s3 = "" )
      // os is hint; implementation may ignore, particularly in embedded systems
      : m_os(os), m_s1(s1), m_s2(s2), m_s3(s3) { restart(expected_count); }

      explicit progress_display( 
         std::ostream & os = std::cout,
         const std::string & s1 = "\n", //leading strings
         const std::string & s2 = "",
         const std::string & s3 = "" )
         // os is hint; implementation may ignore, particularly in embedded systems
         : m_os(os), m_s1(s1), m_s2(s2), m_s3(s3) 
      {
         _count = _next_tic_count = _tic = 0;
         _expected_count = 2;   
      }

      void restart( unsigned long expected_count )
         //  Effects: display appropriate scale
         //  Postconditions: count()==0, expected_count()==expected_count
      {
         _count = _next_tic_count = _tic = 0;
         _expected_count = expected_count;

         m_os << m_s1 << "0%   10   20   30   40   50   60   70   80   90   100%\n"
            << m_s2 << "|----|----|----|----|----|----|----|----|----|----|"
            << std::endl  // endl implies flush, which ensures display
            << m_s3;
         if ( _expected_count < 2) _expected_count = 2;  // prevent divide by zero
      } // restart

      unsigned long  operator+=( unsigned long increment )
         //  Effects: Display appropriate progress tic if needed.
         //  Postconditions: count()== original count() + increment
         //  Returns: count().
      {
         if ( (_count += increment) >= _next_tic_count ) { display_tic(); }
         return _count;
      }

      unsigned long  operator++()           { return operator+=( 1 ); }
      unsigned long  count() const          { return _count; }
      unsigned long  expected_count() const { return _expected_count; }

private:
   std::ostream &     m_os;  // may not be present in all imps
   const std::string  m_s1;  // string is more general, safer than 
   const std::string  m_s2;  //  const char *, and efficiency or size are
   const std::string  m_s3;  //  not issues

   unsigned long _count, _expected_count, _next_tic_count;
   unsigned int  _tic;
   void display_tic()
   {
      // use of floating point ensures that both large and small counts
      // work correctly.  static_cast<>() is also used several places
      // to suppress spurious compiler warnings. 
      unsigned int tics_needed =
         static_cast<unsigned int>(static_cast<double>(_count) / (_expected_count - 1)  * 51 );

      for( ; _tic < tics_needed; ++_tic ) { m_os << '*' << std::flush; }

      _next_tic_count = 
         static_cast<unsigned long>(((_tic + 1)/51.0)*(_expected_count - 1));
      
      if ( _count == _expected_count - 1 ) 
      {
         if( _tic < 51 ) { m_os << '*'; ++_tic; }
         m_os << std::endl;
      }
   } // display_tic
};

//Моё дополнение позволяет контролировать позицию в некотором файле и выводить
//индикатор процесса его считывания
template<class Elem = char, class Tr = char_traits<Elem> >
class CFileProgres
{
   typedef std::basic_istream<Elem, Tr> TTestStream;
   typedef typename TTestStream::pos_type TPos;
   
   progress_display m_Progress;
   TTestStream *m_pStream;
   TPos m_PrevPos;

public:
      explicit CFileProgres( 
         std::ostream & os = std::cout,
         const std::string & s1 = "\n", //leading strings
         const std::string & s2 = "",
         const std::string & s3 = "" 
         ): m_Progress( os, s1, s2, s3 ),  m_pStream(0), m_PrevPos(0) 
      {}

      explicit CFileProgres( 
         TTestStream &Stream,
         std::ostream & os = std::cout,
         const std::string & s1 = "\n", //leading strings
         const std::string & s2 = "",
         const std::string & s3 = "" 
         ): m_Progress( os, s1, s2, s3 ),  m_pStream(0), m_PrevPos(0) 
      {  Reset(Stream); }


      void Reset( TTestStream &Stream )
      {
         m_pStream = &Stream;

         m_PrevPos = m_pStream->tellg();
         
         m_pStream->seekg( 0, std::ios_base::end );
         m_Progress.restart( m_pStream->tellg() + static_cast<TPos>(1) );
         m_pStream->seekg( m_PrevPos, std::ios_base::beg );
      }

      void Update()
      {
         TPos CurPos = m_pStream->tellg();
         m_Progress += CurPos - m_PrevPos;
         m_PrevPos = CurPos;
      }

      void EndTask()
      {
         m_PrevPos = m_Progress.expected_count() - 1;
         m_Progress += m_Progress.expected_count() - 1 - m_Progress.count();
      }
};

}} //namespace SS::Lib 

#endif
