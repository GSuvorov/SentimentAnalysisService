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

namespace OffsetsBases.TonalityMarking
{
    public class Offsets : OffsetsBase
    {
        #region [.ctor().]
        public Offsets()
        {
        }
        #endregion

        #region [.Properties.]
        public OffsetPair[] Objects
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
                case TextFormat.Xml:  return (OffsetsXmlSerializer .ToXmlString ( this ));
                case TextFormat.Text: return (OffsetsTextSerializer.ToTextString( this ));
                default:
                    throw (new ArgumentException(textFormat.ToString()));
            }
        }
        public static Offsets FromString( string offsetsXml, TextFormat textFormat = TextFormat.Text )
        {
            switch (textFormat)
            {
                case TextFormat.Xml:  return (OffsetsXmlSerializer .FromXmlString ( offsetsXml ));
                case TextFormat.Text: return (OffsetsTextSerializer.FromTextString( offsetsXml ));
                default:
                    throw (new ArgumentException(textFormat.ToString()));
            }
        }
    }
    //------------------------------------------------------------------//

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
            var xd = XDocument.Parse( reader.ReadOuterXml() );

            offsets.Sentence         = ReadXmlOffsetPair( xd, SENTENCE_TAG, true );
            offsets.Objects          = ReadXmlOffsetPairs( xd, OBJECT_TAG );
            offsets.PositiveTonality = ReadXmlOffsetPairTonalitys( xd, POSITIVETONALITY_TAG );
            offsets.NegativeTonality = ReadXmlOffsetPairTonalitys( xd, NEGATIVETONALITY_TAG );
            offsets.NeutralTonality  = ReadXmlOffsetPairs( xd, NEUTRALTONALITY_TAG );
            offsets.Anaphoria        = ReadXmlOffsetPairAnaphorias( xd, ANAPHORIA_TAG );
        }
        private static void WriteXml( Offsets offsets, XmlWriter writer )
        {
            WriteXmlOffsetPair( writer, offsets.Sentence, SENTENCE_TAG );
            WriteXmlOffsetPair( writer, offsets.Objects, OBJECT_TAG );
            WriteXmlOffsetPair( writer, offsets.PositiveTonality, POSITIVETONALITY_TAG );
            WriteXmlOffsetPair( writer, offsets.NegativeTonality, NEGATIVETONALITY_TAG );
            WriteXmlOffsetPair( writer, offsets.NeutralTonality, NEUTRALTONALITY_TAG );
            WriteXmlOffsetPair( writer, offsets.Anaphoria, ANAPHORIA_TAG );
        }
    }

    internal class OffsetsTextSerializer : OffsetsTextSerializerBase
    {
        #region [.const's.]
        private const int FIELD_COUNT = 6;
        #endregion

        private OffsetsTextSerializer()
        {
        }

        internal static Offsets FromTextString( string offsetsText )
        {
            var ofs = new Offsets();

            var fields = offsetsText.Split(new[] { FIELD_SEPARATOR }, StringSplitOptions.None);
            if ( fields.Length < FIELD_COUNT )
            {
                throw (new ArgumentException("offsetsText.Split( new[] { FIELD_SEPARATOR }, StringSplitOptions.RemoveEmptyEntries ) < " + FIELD_COUNT));
            }

            ofs.Sentence         = ReadOffsetPair          ( fields[ 0 ], true );
            ofs.Objects          = ReadOffsetPairs         ( fields[ 1 ] );
            ofs.PositiveTonality = ReadOffsetPairTonalitys ( fields[ 2 ] );
            ofs.NegativeTonality = ReadOffsetPairTonalitys ( fields[ 3 ] );
            ofs.NeutralTonality  = ReadOffsetPairs         ( fields[ 4 ] );
            ofs.Anaphoria        = ReadOffsetPairAnaphorias( fields[ 5 ] );

            return (ofs);
        }
        public static string ToTextString( Offsets offsets )
        {
            var sb = new StringBuilder();

            WriteOffsetPair( sb, offsets.Sentence         ); sb.Append(FIELD_SEPARATOR);
            WriteOffsetPair( sb, offsets.Objects          ); sb.Append(FIELD_SEPARATOR);
            WriteOffsetPair( sb, offsets.PositiveTonality ); sb.Append(FIELD_SEPARATOR);
            WriteOffsetPair( sb, offsets.NegativeTonality ); sb.Append(FIELD_SEPARATOR);
            WriteOffsetPair( sb, offsets.NeutralTonality  ); sb.Append(FIELD_SEPARATOR);
            WriteOffsetPair( sb, offsets.Anaphoria        ); sb.Append(FIELD_SEPARATOR);

            return (sb.ToString());
        }       
    }
    //------------------------------------------------------------------//

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

            #region [.Objects.]
            offsets.Objects = Get4Objects();
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

        private OffsetPair[] Get4Objects()
        {
            var @objects = from o in _Sent.DescendantsEntity()
                           where o.IsINQUIRYINITIAL()
                           select o;

            var list = new Lazy< List< OffsetPair > >( LazyThreadSafetyMode.None );
            foreach ( var @object in @objects )
            {
                var op = Get4ObjectSubject( @object );
                if ( op.IsNull() )
                    continue;

                list.Value.Add( op );
            }

            return (list.IsValueCreated ? list.Value.ToArray() : null);
        }
    }

    public static class OffsetsString2XDocumentConverter
    {
        public static XDocument ConvertToFlat( string originalText, string offsetsString, TextFormat textFormat = TextFormat.Text )
        {
            originalText .ThrowIfEmptyOrNull("originalText" );
            offsetsString.ThrowIfEmptyOrNull("offsetsString");

            var offsets = Offsets.FromString( offsetsString, textFormat );

            #region [.create point's.]
            var points = point.createFrom( offsets.Objects, tag_type.@object )
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
            var points = point.createFrom( offsets.Objects, tag_type.@object )
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
