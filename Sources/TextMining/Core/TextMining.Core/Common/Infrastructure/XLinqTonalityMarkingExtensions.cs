using System;
using System.Linq;
using System.Xml.Linq;

using TonalityMarking;

namespace TextMining.Core
{
    public static partial class XLinqExtensions
    {
        public static TonalityType GetAttributeSNT( this XElement element )
        {
            return (element.AttributeValue2Enum< TonalityType >( Consts.SNT_ATTRIBUTE ));
        }
        public static int GetAttributeFRT( this XElement element )
        {
            return (element.GetAttribute( Consts.FRT_ATTRIBUTE ).TryConvert2Int().GetValueOrDefault( 0 ));            
        }

        public static TonalityType GetAttributeSNTFINAL( this XElement element )
        {
            return (element.AttributeValue2Enum< TonalityType >( Consts.SNTFINAL_ATTRIBUTE ));
        }
        public static TonalityType? TryGetAttributeSNTFINAL( this XElement element )
        {
            return (element.GetAttribute( Consts.SNTFINAL_ATTRIBUTE ).TryConvert2Enum< TonalityType >());
        }
        public static int GetAttributeFRTFINAL( this XElement element )
        {
            return (element.GetAttribute( Consts.FRTFINAL_ATTRIBUTE ).TryConvert2Int().GetValueOrDefault( 0 ));  
        }

        public static void SetAttributeSNT( this XElement element, Nouns? type )
        {
            element.SetAttribute( Consts.SNT_ATTRIBUTE, type.GetValueOrDefault( Nouns.nNeut ).ToText() );
        }
        public static void SetAttributeSNT( this XElement element, Verbs? type )
        {
            element.SetAttribute( Consts.SNT_ATTRIBUTE, type.GetValueOrDefault( Verbs.vNeut ).ToText() );
        }
        public static void SetAttributeSNT( this XElement element, Adjectives? type )
        {
            element.SetAttribute( Consts.SNT_ATTRIBUTE, type.GetValueOrDefault( Adjectives.adjNeut ).ToText() );
        }
        public static void SetAttributeSNT( this XElement element, Adverbs? type )
        {
            element.SetAttribute( Consts.SNT_ATTRIBUTE, type.GetValueOrDefault( Adverbs.advNeut ).ToText() );
        }
        public static void SetAttributeSNT( this XElement element, Collocations? type )
        {
            element.SetAttribute( Consts.SNT_ATTRIBUTE, type.GetValueOrDefault( Collocations.collNeut ).ToText() );
        }
        public static void SetAttributeSNT( this XElement element, TonalityType type )
        {
            element.SetAttribute( Consts.SNT_ATTRIBUTE, type.ToText() );
        }

        public static void SetAttributeFRT( this XElement element, int value )
        {
            element.SetAttribute( Consts.FRT_ATTRIBUTE, value.ToString() );
        }
        public static void SetAttributeSNTFINAL( this XElement element, TonalityType type )
        {
            element.SetAttribute( Consts.SNTFINAL_ATTRIBUTE, type.ToText() );
        }
        public static void SetAttributeFRTFINAL( this XElement element, int frt )
        {
            element.SetAttribute( Consts.FRTFINAL_ATTRIBUTE, frt.ToString() );
        }

        private static void SetAttributeSNTnFRT4SelfAndElements2Neutral( this XElement element )
        {
            element.SetAttributeSNTnFRT4SelfAndElements( TonalityType.nNeut );
        }
        private static void SetAttributeSNTnFRT4SelfAndElements( this XElement element, TonalityType? type )
        {
            if ( !element.IsElementT() && !element.IsElementUorCET() )
            {
                element.SetAttributeSNT( type.GetValueOrDefault( TonalityType.nNeut ) );
                element.SetAttributeFRT( 0                                            );//type.ToFRT() );

                foreach ( var child in element.ElementsExceptT() )
                {
                    child.SetAttributeSNTnFRT4SelfAndElements( type );
                }
            }
        }
        public static void MarkAsInquiry( this XElement element )
        {
            element.SetAttribute( Consts.INQUIRYINITIAL_ATTRIBUTE, Consts.TRUE );

            var parentENTITYOrVERBs = from parent in element.AncestorsAndSelf()
                                      where parent.IsElementENTITY() || parent.IsElementVERB()
                                      select parent;

            foreach ( var parent in parentENTITYOrVERBs )
            {
                parent.SetAttributeTYPE( TypeAttributeValue.Inquiry );
            }
            parentENTITYOrVERBs.Last().SetAttributeSNTnFRT4SelfAndElements( TonalityType.nNeut );

            //new!
            foreach ( var d in element.DescendantsExceptT() )
            {
                if ( !d.IsElementUorCET() )
                {
                    d.SetAttributeTYPE( TypeAttributeValue.Inquiry );
                }
            }
        }
        public static void MarkAsInquiryHomogeneous( this XElement element )
        {
            element.SetAttribute( Consts.INQUIRYINITIAL_ATTRIBUTE, Consts.TRUE );

            var parentENTITYOrVERBs = from parent in element.AncestorsAndSelf()
                                      where parent.IsElementENTITY() || parent.IsElementVERB()
                                      select parent;

            foreach ( var parent in parentENTITYOrVERBs )
            {
                parent.SetAttributeTYPE( TypeAttributeValue.Inquiry );
            }
            parentENTITYOrVERBs.Last().SetAttributeSNTnFRT4SelfAndElements( TonalityType.nNeut );

            /*//new!
            foreach ( var d in element.DescendantsExceptT() )
            {
                if ( !d.IsElementUorCET() )
                {
                    d.SetAttributeTYPE( TypeAttributeValue.Inquiry );
                }
            }*/
        }
    }
}