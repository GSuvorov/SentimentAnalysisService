#ifndef MultipleCompare_HPP
#define MultipleCompare_HPP

namespace SS { namespace Lib {

///////////////////////////////////////////////////////////////////////////////
// Набор перегруженный функций которые определяют порядок сортировки некоторого
// набора данных по нескольким полям сразу 
///////////////////////////////////////////////////////////////////////////////
template< class Value1, class Compare1 >
inline bool MultipleCompare( 
                            const Value1 &V11, const Value1 &V12, 
                            Compare1 Cmp1 
                            )
{
   return Cmp1( V11, V12 );
}
///////////////////////////////////////////////////////////////////////////////

template< class Value1, class Value2, class Compare1, class Compare2 >
inline bool MultipleCompare( 
                            const Value1 &V11, const Value1 &V12,
                            const Value2 &V21, const Value2 &V22,
                            Compare1 Cmp1,
                            Compare2 Cmp2
                            )
{
   return 
      Cmp1( V11, V12 ) || (!Cmp1( V12, V11 ) && Cmp2(V21, V22));
}
///////////////////////////////////////////////////////////////////////////////

template< class Value1, class Value2, class Value3, class Compare1, class Compare2, class Compare3 >
inline bool MultipleCompare( 
                            const Value1 &V11, const Value1 &V12,
                            const Value2 &V21, const Value2 &V22,
                            const Value3 &V31, const Value3 &V32,
                            Compare1 Cmp1,
                            Compare2 Cmp2,
                            Compare3 Cmp3
                            )
{
   return 
      Cmp1( V11, V12 ) || 
      (
         !Cmp1( V12, V11 ) && 
         (
            Cmp2(V21, V22) ||
            (
               !Cmp2(V22, V21 ) && Cmp3(V31, V32 )   
            )
         )
      );
}
///////////////////////////////////////////////////////////////////////////////

template< 
   class Value1, class Value2, class Value3, class Value4,
   class Compare1, class Compare2, class Compare3, class Compare4 
>
inline bool MultipleCompare( 
                            const Value1 &V11, const Value1 &V12,
                            const Value2 &V21, const Value2 &V22,
                            const Value3 &V31, const Value3 &V32,
                            const Value4 &V41, const Value4 &V42,
                            Compare1 Cmp1,
                            Compare2 Cmp2,
                            Compare3 Cmp3,
                            Compare4 Cmp4
                            )
{
   return 
      Cmp1( V11, V12 ) || 
      (
         !Cmp1( V12, V11 ) && 
         (
            Cmp2(V21, V22) ||
            (
               !Cmp2(V22, V21 ) &&
               (
                  Cmp3(V31, V32 ) ||
                  (
                     !Cmp3(V32, V31 ) && Cmp4(V41, V42)  
                  )
               )
            )
         )
      );
}
///////////////////////////////////////////////////////////////////////////////

template< 
   class Value1, class Value2, class Value3, class Value4, class Value5,
   class Compare1, class Compare2, class Compare3, class Compare4, class Compare5
>
inline bool MultipleCompare( 
                            const Value1 &V11, const Value1 &V12,
                            const Value2 &V21, const Value2 &V22,
                            const Value3 &V31, const Value3 &V32,
                            const Value4 &V41, const Value4 &V42,
                            const Value5 &V51, const Value5 &V52,
                            Compare1 Cmp1,
                            Compare2 Cmp2,
                            Compare3 Cmp3,
                            Compare4 Cmp4,
                            Compare5 Cmp5
                            )
{
   return 
      Cmp1( V11, V12 ) || 
      (
         !Cmp1( V12, V11 ) && 
         (
            Cmp2(V21, V22) ||
            (
               !Cmp2(V22, V21 ) &&
               (
                  Cmp3(V31, V32 ) ||
                  (
                     !Cmp3(V32, V31 ) && 
                     (
                        Cmp4(V41, V42) ||
                        (
                           !Cmp4(V42, V41) && Cmp5(V51, V52)
                        )
                     )
                  )
               )
            )
         )
      );
}
///////////////////////////////////////////////////////////////////////////////

template< 
   class Value1, class Value2, class Value3, class Value4, class Value5, class Value6,
   class Compare1, class Compare2, class Compare3, class Compare4, class Compare5, class Compare6
>
inline bool MultipleCompare( 
                            const Value1 &V11, const Value1 &V12,
                            const Value2 &V21, const Value2 &V22,
                            const Value3 &V31, const Value3 &V32,
                            const Value4 &V41, const Value4 &V42,
                            const Value5 &V51, const Value5 &V52,
                            const Value6 &V61, const Value6 &V62,
                            Compare1 Cmp1,
                            Compare2 Cmp2,
                            Compare3 Cmp3,
                            Compare4 Cmp4,
                            Compare5 Cmp5,
                            Compare6 Cmp6
                            )
{
   return 
      Cmp1( V11, V12 ) || 
      (
         !Cmp1( V12, V11 ) && 
         (
            Cmp2(V21, V22) ||
            (
               !Cmp2(V22, V21 ) &&
               (
                  Cmp3(V31, V32 ) ||
                  (
                     !Cmp3(V32, V31 ) && 
                     (
                        Cmp4(V41, V42) ||
                        (
                           !Cmp4(V42, V41) && 
                           (
                              Cmp5(V51, V52) ||
                              (
                                 !Cmp5(V52, V52) && Cmp6(V61, V62)
                              )
                           )
                        )
                     )
                  )
               )
            )
         )
      );
}
///////////////////////////////////////////////////////////////////////////////

template< 
   class Value1, class Value2, class Value3, class Value4, class Value5, class Value6, class Value7,
   class Compare1, class Compare2, class Compare3, class Compare4, class Compare5, class Compare6, class Compare7
>
inline bool MultipleCompare( 
                            const Value1 &V11, const Value1 &V12,
                            const Value2 &V21, const Value2 &V22,
                            const Value3 &V31, const Value3 &V32,
                            const Value4 &V41, const Value4 &V42,
                            const Value5 &V51, const Value5 &V52,
                            const Value6 &V61, const Value6 &V62,
                            const Value7 &V71, const Value7 &V72,
                            Compare1 Cmp1,
                            Compare2 Cmp2,
                            Compare3 Cmp3,
                            Compare4 Cmp4,
                            Compare5 Cmp5,
                            Compare6 Cmp6,
                            Compare7 Cmp7
                            )
{
   return 
      Cmp1( V11, V12 ) || 
      (
         !Cmp1( V12, V11 ) && 
         (
            Cmp2(V21, V22) ||
            (
               !Cmp2(V22, V21 ) &&
               (
                  Cmp3(V31, V32 ) ||
                  (
                     !Cmp3(V32, V31 ) && 
                     (
                        Cmp4(V41, V42) ||
                        (
                           !Cmp4(V42, V41) && 
                           (
                              Cmp5(V51, V52) ||
                              (
                                 !Cmp5(V52, V52) && 
                                 (
                                    Cmp6(V61, V62) ||
                                    (
                                       !Cmp6(V62, V61) && Cmp7(V71, V72)
                                    )
                                 )
                              )
                           )
                        )
                     )
                  )
               )
            )
         )
      );
}

}} //namespace SS::Lib 

#endif