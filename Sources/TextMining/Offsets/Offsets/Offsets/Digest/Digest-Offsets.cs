using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.IO;
using System.Linq;
using System.Runtime.Serialization;
using System.Text;
using System.Threading;
using System.Xml;
using System.Xml.Linq;
using System.Xml.Schema;
using System.Xml.Serialization;
using System.Xml.XPath;

using point      = OffsetsBases.OffsetsString2XDocumentConverterHelper.point;
using point_type = OffsetsBases.OffsetsString2XDocumentConverterHelper.point_type;
using tag_type   = OffsetsBases.OffsetsString2XDocumentConverterHelper.tag_type;

//using point_pair = OffsetsBases.OffsetsString2XDocumentConverterHelper.point_pair;

namespace OffsetsBases.Digest
{
    /// <summary>
    /// 
    /// </summary>
    public class Offsets : OffsetsBase
    {
        #region [.ctor().]
        public Offsets()
        {
        }
        #endregion

        #region [.Properties.]
        public OffsetPair Subject
        {
            get;
            set;
        }
        public OffsetPair Object
        {
            get;
            set;
        }

        public OffsetPair[] Indirectspeech
        {
            get;
            set;
        }
        public OffsetPair[] Directspeech
        {
            get;
            set;
        }
        #endregion

        public override string ToString()
        {
            return (this.ToString( TextFormat.Text ));
        }
        public override string ToString( TextFormat textFormat = TextFormat.Text )
        {
            switch (textFormat)
            {
                case TextFormat.Xml:  return (OffsetsXmlSerializer.ToXmlString(this));
                case TextFormat.Text: return (OffsetsTextSerializer.ToTextString(this));
                default:
                    throw (new ArgumentException(textFormat.ToString()));
            }
        }
        public static Offsets FromString( string offsetsXml, TextFormat textFormat = TextFormat.Text )
        {
            switch ( textFormat )
            {
                case TextFormat.Xml:  return (OffsetsXmlSerializer .FromXmlString ( offsetsXml ));
                case TextFormat.Text: return (OffsetsTextSerializer.FromTextString( offsetsXml ));
                default:
                    throw (new ArgumentException(textFormat.ToString()));
            }
        }
    }
    //------------------------------------------------------------------//

    /// <summary>
    /// 
    /// </summary>
    internal class OffsetsXmlSerializer : OffsetsXmlSerializerBase
    {
        private OffsetsXmlSerializer()
        {
        }

        internal static Offsets FromXmlString( string offsetsXml )
        {
            var ofs = new Offsets();

            using (var sr = new StringReader(offsetsXml))
            using (var xr = new XmlTextReader(sr))
            {
                xr.Read();

                ReadXml(ofs, xr);
            }

            return (ofs);
        }
        public static string ToXmlString( Offsets offsets )
        {
            var sb = new StringBuilder();
            using (var sw = new StringWriter(sb))
            using (var xw = new XmlTextWriter(sw) { Formatting = Formatting.None })
            {
                xw.WriteStartElement( ROOT_TAG );
                WriteXml(offsets, xw);
                xw.WriteEndElement();
            }
            return (sb.ToString());
        }

        private static void ReadXml( Offsets offsets, XmlReader reader )
        {
            var xd = XDocument.Parse(reader.ReadOuterXml());

            offsets.Sentence = ReadXmlOffsetPair(xd, SENTENCE_TAG, true);
            offsets.Subject = ReadXmlOffsetPair(xd, SUBJECT_TAG);
            offsets.Object = ReadXmlOffsetPair(xd, OBJECT_TAG);
            offsets.Indirectspeech = ReadXmlOffsetPairs(xd, INDIRECTSPEECH_TAG);
            offsets.Directspeech = ReadXmlOffsetPairs(xd, DIRECTSPEECH_TAG);
            offsets.PositiveTonality = ReadXmlOffsetPairTonalitys(xd, POSITIVETONALITY_TAG);
            offsets.NegativeTonality = ReadXmlOffsetPairTonalitys(xd, NEGATIVETONALITY_TAG);
            offsets.NeutralTonality = ReadXmlOffsetPairs(xd, NEUTRALTONALITY_TAG);
            offsets.Anaphoria = ReadXmlOffsetPairAnaphorias(xd, ANAPHORIA_TAG);
        }
        private static void WriteXml( Offsets offsets, XmlWriter writer )
        {
            WriteXmlOffsetPair(writer, offsets.Sentence, SENTENCE_TAG);
            WriteXmlOffsetPair(writer, offsets.Subject, SUBJECT_TAG);
            WriteXmlOffsetPair(writer, offsets.Object, OBJECT_TAG);
            WriteXmlOffsetPair(writer, offsets.Indirectspeech, INDIRECTSPEECH_TAG);
            WriteXmlOffsetPair(writer, offsets.Directspeech, DIRECTSPEECH_TAG);
            WriteXmlOffsetPair(writer, offsets.PositiveTonality, POSITIVETONALITY_TAG);
            WriteXmlOffsetPair(writer, offsets.NegativeTonality, NEGATIVETONALITY_TAG);
            WriteXmlOffsetPair(writer, offsets.NeutralTonality, NEUTRALTONALITY_TAG);
            WriteXmlOffsetPair(writer, offsets.Anaphoria, ANAPHORIA_TAG);
        }
    }

    /// <summary>
    /// 
    /// </summary>
    internal class OffsetsTextSerializer : OffsetsTextSerializerBase
    {
        #region [.const's.]
        private const int FIELD_COUNT = 9;
        #endregion

        private OffsetsTextSerializer()
        {
        }

        internal static Offsets FromTextString( string offsetsText )
        {
            var ofs = new Offsets();

            var fields = offsetsText.Split( new[] { FIELD_SEPARATOR }, StringSplitOptions.None );
            if ( fields.Length < FIELD_COUNT )
            {
                throw (new ArgumentException("offsetsText.Split( new[] { FIELD_SEPARATOR }, StringSplitOptions.RemoveEmptyEntries ) < " + FIELD_COUNT));
            }

            ofs.Sentence         = ReadOffsetPair          ( fields[ 0 ], true );
            ofs.Subject          = ReadOffsetPair          ( fields[ 1 ] );
            ofs.Object           = ReadOffsetPair          ( fields[ 2 ] );
            ofs.Indirectspeech   = ReadOffsetPairs         ( fields[ 3 ] );
            ofs.Directspeech     = ReadOffsetPairs         ( fields[ 4 ] );
            ofs.PositiveTonality = ReadOffsetPairTonalitys ( fields[ 5 ] );
            ofs.NegativeTonality = ReadOffsetPairTonalitys ( fields[ 6 ] );
            ofs.NeutralTonality  = ReadOffsetPairs         ( fields[ 7 ] );
            ofs.Anaphoria        = ReadOffsetPairAnaphorias( fields[ 8 ] );

            return (ofs);
        }
        public static string ToTextString( Offsets offsets )
        {
            var sb = new StringBuilder();

            WriteOffsetPair( sb, offsets.Sentence        ); sb.Append(FIELD_SEPARATOR);
            WriteOffsetPair( sb, offsets.Subject         ); sb.Append(FIELD_SEPARATOR);
            WriteOffsetPair( sb, offsets.Object          ); sb.Append(FIELD_SEPARATOR);
            WriteOffsetPair( sb, offsets.Indirectspeech  ); sb.Append(FIELD_SEPARATOR);
            WriteOffsetPair( sb, offsets.Directspeech    ); sb.Append(FIELD_SEPARATOR);
            WriteOffsetPair( sb, offsets.PositiveTonality); sb.Append(FIELD_SEPARATOR);
            WriteOffsetPair( sb, offsets.NegativeTonality); sb.Append(FIELD_SEPARATOR);
            WriteOffsetPair( sb, offsets.NeutralTonality ); sb.Append(FIELD_SEPARATOR);
            WriteOffsetPair( sb, offsets.Anaphoria       ); sb.Append(FIELD_SEPARATOR);

            return (sb.ToString());
        }       
    }
    //------------------------------------------------------------------//

    /// <summary>
    /// 
    /// </summary>
    public class OffsetsProcessor : OffsetsProcessorBase< Offsets >
    {
#if Create_NodeOffsetMap_Over_Mikl_Algorithm
        public OffsetsProcessor( string originalText ) 
            : base( originalText )
        {
        }
#else
        public OffsetsProcessor() 
            : base()
        {
        }
#endif

        protected override Offsets CreateOffsets()
        {
            var offsets = new Offsets();

            #region [.Sentence.]
            offsets.Sentence = Get4Sentence();
            #endregion

            #region [.Object.]
            var @object = _Sent.DescendantsEntity().FirstOrDefault( e => e.IsINQUIRYINITIAL() );
            offsets.Object = Get4ObjectSubject( @object );
            #endregion

            #region [.Subject.]
            var subject = _Sent.DescendantsEntity().FirstOrDefault( e => e.ISSUBJECT() );
            offsets.Subject = Get4ObjectSubject( subject );
            #endregion

            #region [.Indirectspeech & Directspeech.]
            offsets.Directspeech = Get4ISDSSpeech( Extensions4Offsets.DIRECTSPEECH_BEGIN,
                                                   Extensions4Offsets.DIRECTSPEECH_END );
            offsets.Indirectspeech = Get4ISDSSpeech( Extensions4Offsets.INDIRECTSPEECH_BEGIN,
                                                     Extensions4Offsets.INDIRECTSPEECH_END );
            #endregion

            #region [.Tonality.]
            var t = Get4Tonality();

            offsets.PositiveTonality = t.Item1;
            offsets.NegativeTonality = t.Item2;
            offsets.NeutralTonality  = t.Item3;
            #endregion

            #region [.Anaphoria.]
            offsets.Anaphoria = Get4Anaphoria();
            #endregion

            return (offsets);
        }

        private OffsetPair[] Get4ISDSSpeech( string beginAttributeName, string endAttributeName )
        {
            var begin = _Sent.DescendantsAndSelfWithExistsAttribute( beginAttributeName ).ToArray();
            var end   = _Sent.DescendantsAndSelfWithExistsAttribute( endAttributeName   ).ToArray();

            var list = new Lazy< List< OffsetPair > >( LazyThreadSafetyMode.None );
            for ( int i = 0, count = Math.Min( begin.Length, end.Length ); i < count; i++ )
            {
                #region [.begin.]
                var begin_ = begin[ i ];

                var begin_off = getOffsetPair4Element( begin_ );
                if ( begin_off.IsNull() )
                {
                    foreach ( var a in begin_.ElementsAfterSelf() )
                    {
                        begin_off = getOffsetPair4Element( a );
                        if ( begin_off.IsNotNull() )
                            break;
                    }

                    if ( begin_off.IsNull() )
                        continue;
                }
                #endregion

                #region [.end.]
                var end_ = end[ i ];

                var end_off = getOffsetPair4Element( end_ );
                if ( end_off.IsNull() )
                {
                    foreach ( var b in end_.ElementsBeforeSelf().Reverse() )
                    {
                        end_off = getOffsetPair4Element( b );
                        if ( end_off.IsNotNull() )
                            break;
                    }

                    if ( end_off.IsNull() )
                        continue;
                }
                #endregion

                #region [.add 2 list.]
                var len = end_off.BeginIndex - begin_off.BeginIndex + end_off.Length;
                if ( len <= 0 )
                    continue;

                list.Value.Add( OffsetPair.Create( begin_off.BeginIndex, len ) );
                #endregion
            }
            return (list.IsValueCreated ? list.Value.ToArray() : null);
        }

        //private OffsetPair[] Get4ISDSSpeech( string beginAttributeName, string endAttributeName )
        //{
        //    var begin = _Sent.DescendantsAndSelfWithExistsAttribute( beginAttributeName ).ToArray();
        //    var end   = _Sent.DescendantsAndSelfWithExistsAttribute( endAttributeName   ).ToArray();

        //    var list = new Lazy< List< OffsetPair > >( LazyThreadSafetyMode.None );
        //    for ( int i = 0, count = Math.Min( begin.Length, end.Length ); i < count; i++ )
        //    {
        //        #region [.begin.]
        //        var begin_pos = default(int?);

        //        var begin_ = begin[ i ].DescendantsXCData().FirstOrDefault();

        //        if ( begin_.IsNotNull() )
        //             begin_pos = _NodeOffsetMap.TryGetValueEx( begin_ );

        //        if ( !begin_pos.HasValue )
        //        {
        //            var index = Array.IndexOf( _SentDescendantNodesAndSelf, begin_ );
        //            if ( index < 0 )
        //                continue;

        //            for ( int n = index + 1; n < _SentDescendantNodesAndSelf.Length; n++ )
        //            {
        //                var d = _SentDescendantNodesAndSelf[ n ];
        //                var cd = (d.NodeType == XmlNodeType.Element) ? ((XElement) d).DescendantsXCData().FirstOrDefault() : d;
        //                if ( cd.IsNull() )
        //                    continue;
        //                begin_pos = _NodeOffsetMap.TryGetValueEx( cd );
        //                if ( begin_pos.HasValue )
        //                {
        //                    /*
        //                    for ( int k = index; k < n; k++ )
        //                    {
        //                        d = _SentDescendantNodesAndSelf[ k ];
        //                        if ( d.NodeType == XmlNodeType.CDATA )
        //                        {
        //                            begin_pos -= ((XCData) d).Value.Length;
        //                        }
        //                    }
        //                    */
        //                    //*if ( begin_pos.Value < _SentPosition )
        //                    //{
        //                    //    __sentPosition = begin_pos.Value;
        //                    //}*/

        //                    break;
        //                }
        //            }

        //            if ( !begin_pos.HasValue )
        //                continue;
        //        }
        //        #endregion

        //        #region [.end.]
        //        var end_pos = default(int?);

        //        var end_ = end[ i ].DescendantsXCData().LastOrDefault();

        //        if ( end_.IsNotNull() )
        //             end_pos = _NodeOffsetMap.TryGetValueEx( end_ );

        //        if ( !end_pos.HasValue )
        //        {
        //            var index = Array.IndexOf( _SentDescendantNodesAndSelf, end_ );
        //            if ( index < 0 )
        //                continue;

        //            for ( int n = index - 1; 0 <= n; n-- )
        //            {
        //                var d = _SentDescendantNodesAndSelf[ n ];
        //                var cd = (d.NodeType == XmlNodeType.Element) ? ((XElement) d).DescendantsXCData().FirstOrDefault() : d;
        //                if ( cd.IsNull() )
        //                    continue;
        //                end_pos = _NodeOffsetMap.TryGetValueEx( cd );
        //                if ( end_pos.HasValue )
        //                {
        //                    /*
        //                    for ( int k = n; k < index; k++ )
        //                    {
        //                        d = _SentDescendantNodesAndSelf[ k ];
        //                        if ( d.NodeType == XmlNodeType.CDATA )
        //                        {
        //                            end_pos += ((XCData) d).Value.Length;
        //                        }
        //                    }
        //                    */

        //                    break;
        //                }
        //            }

        //            if ( !end_pos.HasValue )
        //                continue;
        //        }
        //        #endregion

        //        #region [.add 2 list.]
        //        var len = end_pos.Value - begin_pos.Value + end_.Value.TrimEnd().Length;
        //        if (len <= 0)
        //            continue;

        //        list.Value.Add( OffsetPair.Create( begin_pos.Value - _SentPosition, len ) );
        //        #endregion
        //    }
        //    return (list.IsValueCreated ? list.Value.ToArray() : null);
        //}
    }

    /// <summary>
    /// 
    /// </summary>
    public static class OffsetsString2XDocumentConverter
    {
        public static XDocument ConvertToFlat( string originalText, string offsetsString, TextFormat textFormat = TextFormat.Text )
        {
            originalText .ThrowIfEmptyOrNull("originalText" );
            offsetsString.ThrowIfEmptyOrNull("offsetsString");

            var offsets = Offsets.FromString( offsetsString, textFormat );            

            #region [.create point's.]
            var points = point.createFrom( offsets.Subject, tag_type.subject )
                        .Concat
                        (
                         point.createFrom( offsets.Object, tag_type.@object )
                        )
                        .Concat
                        (
                         point.createFrom( offsets.Indirectspeech, tag_type.indirectspeech )
                        )
                        .Concat
                        (
                         point.createFrom( offsets.Directspeech, tag_type.directspeech )
                        )
                        .Concat
                        (
                         point.createFrom( offsets.PositiveTonality, tag_type.positivetonality )
                        )
                        .Concat
                        (
                         point.createFrom( offsets.NegativeTonality, tag_type.negativetonality )
                        )
                        .Concat
                        (
                         point.createFrom( offsets.NeutralTonality, tag_type.neutraltonality )
                        )
                        .Concat
                        (
                         point.createFrom( offsets.Anaphoria, tag_type.anaphoria )
                        );
            #endregion

            return (OffsetsString2XDocumentConverterHelper.ConvertToFlat( originalText, offsets, points ));
        }

        public static XDocument ConvertToHierarchy( string originalText, string offsetsString, TextFormat textFormat = TextFormat.Text )
        {
            originalText .ThrowIfEmptyOrNull("originalText" );
            offsetsString.ThrowIfEmptyOrNull("offsetsString");

            var offsets = Offsets.FromString( offsetsString, textFormat );            

            #region [.create point's.]
            var points = point.createFrom( offsets.Subject, tag_type.subject )
                        .Concat
                        (
                         point.createFrom( offsets.Object, tag_type.@object )
                        )
                        .Concat
                        (
                         point.createFrom( offsets.Indirectspeech, tag_type.indirectspeech )
                        )
                        .Concat
                        (
                         point.createFrom( offsets.Directspeech, tag_type.directspeech )
                        )
                        .Concat
                        (
                         point.createFrom( offsets.PositiveTonality, tag_type.positivetonality )
                        )
                        .Concat
                        (
                         point.createFrom( offsets.NegativeTonality, tag_type.negativetonality )
                        )
                        .Concat
                        (
                         point.createFrom( offsets.NeutralTonality, tag_type.neutraltonality )
                        )
                        .Concat
                        (
                         point.createFrom( offsets.Anaphoria, tag_type.anaphoria )
                        );
            #endregion

            return (OffsetsString2XDocumentConverterHelper.ConvertToHierarchy( originalText, offsets, points ));
        }
    }
    //------------------------------------------------------------------//
}
