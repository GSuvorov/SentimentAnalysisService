using System;

namespace TonalityMarking
{
    internal static partial class TonalityTypeExtensions
    {
        public static bool IsVerbCollocation( this Collocations collocationType )
        {
            switch ( collocationType )
            {
                case Collocations.collvNeg:
                case Collocations.collvPos:
                case Collocations.collvNeut:
                    return (true);
                default:
                    return (false);
            }            
        }

        public static int ToFRT< T >( this OneWordWeightDictionaryResult< T >? typeAndweight )
            where T : struct
        {
            return (typeAndweight.HasValue ? typeAndweight.Value.Weight : 0);
        }
        public static T ToSNT< T >( this OneWordWeightDictionaryResult< T >? typeAndweight, T defaultvalue )
            where T : struct
        {
            return (typeAndweight.HasValue ? typeAndweight.Value.DictionaryType : defaultvalue);
        }
    }
}




