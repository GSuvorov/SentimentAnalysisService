using System;
using System.Collections.Concurrent;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Threading.Tasks;
using System.Xml.Linq;
using System.Xml.XPath;

using OffsetsBases.TonalityMarking;
using TextMining.Core;
using TextMining.Service;

namespace TonalityMarking.Socview
{
    /// <summary>
    /// 
    /// </summary>
    internal static class SocviewProcessing
    {
        public static SocviewProcessingData Processing( TonalityMarkingWcfService tonalityMarking, SocviewProcessingData socviewData )
        {            
            return (new ParallelSocviewProcessing( tonalityMarking ).Processing( socviewData ));
        }
    }

    /// <summary>
    /// 
    /// </summary>
    internal class ParallelSocviewProcessing
    {
        private const ObjectAllocateMethod OBJECT_ALLOCATE_METHOD = ObjectAllocateMethod.FirstVerbEntityWithRoleObj;

        private TonalityMarkingWcfService _TonalityMarking;
        private ConcurrentDictionary< string, int > _ObjectsNamesDictionary;

        public ParallelSocviewProcessing( TonalityMarkingWcfService tonalityMarking )
        {
            tonalityMarking.ThrowIfNull("tonalityMarking");

            _TonalityMarking = tonalityMarking;
            _ObjectsNamesDictionary = new ConcurrentDictionary< string, int >();
        }

        public SocviewProcessingData Processing( SocviewProcessingData socviewData )
        {
            socviewData.ThrowIfNull("socviewData");

            //1.
            var socviewMessages = socviewData.GetUnprocessedSocviewMessages().ToArray();
            Parallel.ForEach( socviewMessages, ProcessSocviewMessage_1 );

            //2.
            Parallel.ForEach( socviewMessages, ProcessSocviewMessage_2 );

            //3.
            socviewData.SumNeg    += socviewData.SocviewPost.CalcSumNeg();
            socviewData.SumNegFRT += socviewData.SocviewPost.CalcSumNegFRT();
            socviewData.SumPos    += socviewData.SocviewPost.CalcSumPos();
            socviewData.SumPosFRT += socviewData.SocviewPost.CalcSumPosFRT();

            return (socviewData);
        }

        private void ProcessSocviewMessage_1( SocviewMessage socviewMessage )
        {
            if ( socviewMessage.IsProcessed )
                throw (new InvalidOperationException("socviewMessage.IsProcessed"));

            //1.
            var t = _TonalityMarking.ExecuteTonalityMarking4SocviewCallback( socviewMessage.PlainText );
            var xdocument       = t.Item1;
            socviewMessage.CoreferenceInfo = t.Item2;

            //2.
            socviewMessage.Objects = xdocument.ExtractObjects( OBJECT_ALLOCATE_METHOD, socviewMessage.CoreferenceInfo );
            foreach ( var @object in socviewMessage.Objects )
            {
                if ( Algorithms.IsObjectEntity( @object.Element ) )
                    _ObjectsNamesDictionary.AddOrUpdate( @object.Name, 1, (e, i) => i + 1 );
                else
                    _ObjectsNamesDictionary.TryAdd( @object.Name, 1 );
            }

            //3.
            socviewMessage.Sents = (from sent in xdocument
                                                .Root.Elements( TextMining.Core.XLinqExtensions.Consts.SENT_ELEMENT ) //.XPathSelectElements("TEXT/SENT")
                                      where sent.Descendants().Any( d => d.IsTypeInquiry() && 
                                                                         d.HasAttributeSNTFINAL() )
                                    select 
                                        new SocviewSent() 
                                        {
                                            Element = sent,
                                        }
                                    )
                                    .ToArray();

            //4.
            var offsetsProcessor = new OffsetsProcessor( /*socviewMessage.PlainText*/ );
            foreach ( var sent in socviewMessage.Sents )
            {
                try
                {
                    sent.Offsets = offsetsProcessor.CreateOffsetsString( sent.Element );
                }
                catch ( Exception ex )
                {
                    sent.Offsets = ex.ToString();
                }
            }
        }

        private void ProcessSocviewMessage_2( SocviewMessage socviewMessage )
        {
            var count = default(int);

            foreach ( var sent in socviewMessage.Sents )
            {
                var tm_objects = (from tmo in sent.Element.ExtractObjects( OBJECT_ALLOCATE_METHOD, socviewMessage.CoreferenceInfo )
                                    where ( tmo.Negative.HasValue || tmo.Positive.HasValue )
                                  select tmo
                                 )
                                 .ToArray();
                #region [.check.]
                Debug.Assert(tm_objects.Length != 0, "tm_objects.Length == 0");
                if (tm_objects.Length == 0)
                    throw (new InvalidOperationException("tm_objects.Length == 0")); 
                #endregion

                var frt_final_salt = IsSentEndsWithExclamationPoint( sent.Element ) ? 1 : 0;
                
                foreach ( var tm_object in tm_objects )
                {
                    var r = _ObjectsNamesDictionary.TryGetValue( tm_object.Name, out count );
                    #region [.check.]
                    Debug.Assert(r, "!_ObjectsNamesDictionary.TryGetValue( tm_object.Name, out  count )");
                    if (!r)
                        throw (new InvalidOperationException("!_ObjectsNamesDictionary.TryGetValue( tm_object.Name, out  count )")); 
                    #endregion

                    var e = tm_object.GetHomogeneousElementOrElement();
                    var frt_final = ((e.GetAttributeFRTFINAL() + 1) * count) + frt_final_salt;
                    #region
                    /*if ( IsSentEndsWithExclamationPoint( tm_object.Element.GetAncestorSent() ) )
                    {
                        frt_final++;
                    }*/
                    #endregion
                    if ( sent.FRT < frt_final )
                    {
                        sent.FRT = frt_final;
                    }

                    var snt_final = e.GetAttributeSNTFINAL();
                    if ( snt_final.ToString().Contains( "Pos" ) )
                    {
                        sent.SumPosFRT += frt_final;
                        sent.SumPos++;
                    }
                    else
                    {
                        sent.SumNegFRT += frt_final;
                        sent.SumNeg++;
                    }
                }
            }
        }


        private static bool IsSentEndsWithExclamationPoint( XElement sent )
        {
            if ( sent != null )
            {
                //sent.ThrowIfNotSent();

                var last = sent.Descendants().LastOrDefault();
                return (last.IsNotNull() ? last.Value.TrimWhiteSpaces().EndsWith( "!" ) : false);
            }
            return (false);
        }
    }
}