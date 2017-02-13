using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Xml.Linq;

#if WITH_OM_TM
using Digest;
using TonalityMarking;
#endif

namespace Lingvistics
{
#if WITH_OM_TM
	/// <summary>
	/// Объединитель результатов тональности
	/// </summary>
	internal static class TonalityResultUnifier
	{
        public static DigestOutputResult Union( IEnumerable< Tuple< DigestOutputResult, int > > collection )
		{
			int textShift = 0;
            foreach ( var t in collection )
			{
                if ( t.Item1 != null )
				{
                    foreach ( var digestTuple in t.Item1.Tuples )
					{
                        var sentence = XElement.Parse( digestTuple.SentenceText );
                        RDFUnifier.UpdateAttributeValue( sentence, BlockAttribute.POS, textShift );
                        digestTuple.SetSentenceText( sentence.ToString() );
					}
				}
				textShift += t.Item2;
			}
			return (new DigestOutputResult( collection.Where( t => t.Item1 != null ).SelectMany( t => t.Item1.Tuples ) ));
		}

        public static TonalityMarkingOutputResult Union( string rdf, bool rdfIsEmpty, IEnumerable< TonalityMarkingOutputResult > collection )
		{
            var inquiryAllocateStatus = InquiryAllocateStatus.NotAllocated;                  
            var positive = default(double?);
            var negative = default(double?);
            var objects  = new List< TonalityMarkingObject >( 0x100 );
            foreach ( var tmor in collection )
			{
                if ( tmor != null )
				{
                    if ( tmor.InquiryAllocateStatus == InquiryAllocateStatus.Allocated )
                    {
                        inquiryAllocateStatus = InquiryAllocateStatus.Allocated;

                        var atmt = tmor.AuthorTonalityMarkingTuple;
                        objects.AddRange( atmt.Objects );
                        if ( atmt.Positive.HasValue )
                        {
                            positive += atmt.Positive.Value;
                        }
                        if ( atmt.Negative.HasValue )
                        {
                            negative += atmt.Negative.Value;
                        }
                    }
				}
			}
            return (new TonalityMarkingOutputResult( rdf, rdfIsEmpty, inquiryAllocateStatus, positive, negative, objects ));
		}
	}
#endif
}
