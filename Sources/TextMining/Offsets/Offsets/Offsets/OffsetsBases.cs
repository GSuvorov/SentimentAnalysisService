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

namespace OffsetsBases
{
    /// <summary>
    /// 
    /// </summary>
    public enum TextFormat
    {
        Xml,
        Text,
    }
    //------------------------------------------------------------------//

    /// <summary>
    /// 
    /// </summary>
    public class OffsetPair : IEqualityComparer< OffsetPair >
    {
        public OffsetPair( int beginIndex, int length )
        {
            if (beginIndex <  0) throw (new ArgumentException("beginIndex <  0"));
            if (length     <= 0) throw (new ArgumentException("length     <= 0"));

            BeginIndex = beginIndex;
            Length     = length;
        }
        protected OffsetPair()
        {
        }

        public int BeginIndex
        {
            get;
            private set;
        }
        public int Length
        {
            get;
            private set;
        }

        public override string ToString()
        {
            return ("{" + BeginIndex + ";" + Length + "}");
        }
        public override bool Equals( object obj )
        {
            if (obj is OffsetPair)
            {
                var op = (OffsetPair)obj;
                return (BeginIndex == op.BeginIndex &&
                                Length == op.Length);
            }
            return base.Equals(obj);
        }
        public override int GetHashCode()
        {
            return (BeginIndex.GetHashCode() ^ Length.GetHashCode());
        }

        public static OffsetPair Create( int beginIndex, int length )
        {
            return (new OffsetPair(beginIndex, length));
        }

        #region [.IEqualityComparer< OffsetPair >.]
        public static readonly OffsetPair Comparer = new OffsetPair();

        public bool Equals(OffsetPair x, OffsetPair y)
        {
            return (x.BeginIndex <= y.BeginIndex &&
                    y.BeginIndex + y.Length <= x.BeginIndex + x.Length);
        }
        public int GetHashCode(OffsetPair obj)
        {
            return (/*this.GetHashCode()*/ 0);
        }
        #endregion
    }

    /// <summary>
    /// 
    /// </summary>
    public class OffsetPairTonality : OffsetPair, IEqualityComparer< OffsetPairTonality >
    {
        public OffsetPairTonality(int beginIndex, int length, int frt)
            : base(beginIndex, length)
        {
            if (frt < 0) throw (new ArgumentException("frt"));

            FRT = frt;
        }

        public int FRT
        {
            get;
            private set;
        }

        public override string ToString()
        {
            return (base.ToString() + " - " + FRT);
        }

        public static OffsetPairTonality Create(int beginIndex, int length, int frt)
        {
            return (new OffsetPairTonality(beginIndex, length, frt));
        }
        public static OffsetPairTonality Create(OffsetPair op, int frt)
        {
            return (new OffsetPairTonality(op.BeginIndex, op.Length, frt));
        }

        #region [.IEqualityComparer< OffsetPairTonality >.]
        new public static readonly OffsetPairTonality Comparer = new OffsetPairTonality();
        private OffsetPairTonality()
        {
        }

        public bool Equals(OffsetPairTonality x, OffsetPairTonality y)
        {
            if (x.FRT != y.FRT)
                return (false);

            //---return (base.Equals( x, y ));
            return (x.BeginIndex <= y.BeginIndex &&
                    y.BeginIndex + y.Length <= x.BeginIndex + x.Length);
        }
        public int GetHashCode(OffsetPairTonality obj)
        {
            return (/*base.GetHashCode() ^ */ obj.FRT);
        }
        #endregion
    }

    /// <summary>
    /// 
    /// </summary>
    public class OffsetPairAnaphoria : OffsetPair
    {
        public OffsetPairAnaphoria(int beginIndex, int length, string value)
            : base(beginIndex, length)
        {
            if (string.IsNullOrWhiteSpace(value)) throw (new ArgumentException("string.IsNullOrWhiteSpace( value )"));

            Value = value;
        }

        public string Value
        {
            get;
            private set;
        }

        public override string ToString()
        {
            return (base.ToString() + " - " + Value);
        }

        public static OffsetPairAnaphoria Create(int beginIndex, int length, string value)
        {
            return (new OffsetPairAnaphoria(beginIndex, length, value));
        }
        public static OffsetPairAnaphoria Create(OffsetPair op, string value)
        {
            return (new OffsetPairAnaphoria(op.BeginIndex, op.Length, value));
        }
    }

    /// <summary>
    /// 
    /// </summary>
    public abstract class OffsetsBase
    {
        #region [.ctor().]
        public OffsetsBase()
        {
        }
        #endregion

        #region [.Properties.]
        public OffsetPair Sentence
        {
            get;
            set;
        }

        public OffsetPairTonality[] PositiveTonality
        {
            get;
            set;
        }
        public OffsetPairTonality[] NegativeTonality
        {
            get;
            set;
        }
        public OffsetPair[] NeutralTonality
        {
            get;
            set;
        }

        public OffsetPairAnaphoria[] Anaphoria
        {
            get;
            set;
        }
        #endregion

        public abstract string ToString( TextFormat textFormat = TextFormat.Text );
    }
    //------------------------------------------------------------------//

    /// <summary>
    /// 
    /// </summary>
    internal abstract class OffsetsXmlSerializerBase
    {
        #region [.const's.]
        protected const string ROOT_TAG               = "R";
        protected const string SENTENCE_TAG           = "X";
        protected const string SUBJECT_TAG            = "S";
        protected const string OBJECT_TAG             = "O";
        protected const string INDIRECTSPEECH_TAG     = "I";
        protected const string DIRECTSPEECH_TAG       = "D";
        protected const string POSITIVETONALITY_TAG   = "P";
        protected const string NEGATIVETONALITY_TAG   = "N";
        protected const string NEUTRALTONALITY_TAG    = "Z";
        protected const string ANAPHORIA_TAG          = "A";
        protected const string ITEM_TAG               = "T";
        protected const string BEGININDEX_ATTR        = "I";
        protected const string LENGTH_ATTR            = "L";
        protected const string ANAPHORIA_ATTR         = "A";
        protected const string FRT_ATTR               = "F";
        #endregion

        protected OffsetsXmlSerializerBase()
        {
        }

        protected static OffsetPair[] ReadXmlOffsetPairs(XDocument xd, string name)
        {
            var xpath = ROOT_TAG + "/" + name;
            var xe = xd.XPathSelectElement(xpath);
            if (xe == null)
                return (null); //throw (new ArgumentException("Not found node on xpath: " + xpath ));

            var list = new List<OffsetPair>();
            foreach (var e in xe.Elements(ITEM_TAG))
            {
                list.Add(ReadXmlOffsetPair(e));
            }
            if (!list.Any())
                throw (new ArgumentException("Node not has needed child elements, xpath: " + xpath));

            return (list.ToArray());
        }
        protected static OffsetPair   ReadXmlOffsetPair(XElement xe)
        {
            var i = xe.GetAttribute(BEGININDEX_ATTR).TryConvert2Int();
            var l = xe.GetAttribute(LENGTH_ATTR).TryConvert2Int();

            if (i.HasValue && l.HasValue)
                return (OffsetPair.Create(i.Value, l.Value));

            throw (new ArgumentException("Node not has all needed attributes"));
        }
        protected static OffsetPairTonality[] ReadXmlOffsetPairTonalitys(XDocument xd, string name)
        {
            var xpath = ROOT_TAG + "/" + name;
            var xe = xd.XPathSelectElement(xpath);
            if (xe == null)
                return (null); //throw (new ArgumentException("Not found node on xpath: " + xpath ));

            var list = new List<OffsetPairTonality>();
            foreach (var e in xe.Elements(ITEM_TAG))
            {
                list.Add(ReadXmlOffsetPairTonality(e));
            }
            if (!list.Any())
                throw (new ArgumentException("Node not has needed child elements, xpath: " + xpath));

            return (list.ToArray());
        }
        protected static OffsetPairTonality   ReadXmlOffsetPairTonality(XElement xe)
        {
            var i = xe.GetAttribute(BEGININDEX_ATTR).TryConvert2Int();
            var l = xe.GetAttribute(LENGTH_ATTR).TryConvert2Int();
            var f = xe.GetAttribute(FRT_ATTR).TryConvert2Int();

            if (i.HasValue && l.HasValue && f.HasValue)
                return (OffsetPairTonality.Create(i.Value, l.Value, f.Value));

            throw (new ArgumentException("Node not has all needed attributes"));
        }
        protected static OffsetPairAnaphoria[] ReadXmlOffsetPairAnaphorias(XDocument xd, string name)
        {
            var xpath = ROOT_TAG + "/" + name;
            var xe = xd.XPathSelectElement(xpath);
            if (xe == null)
                return (null); //throw (new ArgumentException("Not found node on xpath: " + xpath ));

            var list = new List<OffsetPairAnaphoria>();
            foreach (var e in xe.Elements(ITEM_TAG))
            {
                list.Add(ReadXmlOffsetPairAnaphoria(e));
            }
            if (!list.Any())
                throw (new ArgumentException("Node not has needed child elements, xpath: " + xpath));

            return (list.ToArray());
        }
        protected static OffsetPairAnaphoria   ReadXmlOffsetPairAnaphoria(XElement xe)
        {
            var i = xe.GetAttribute(BEGININDEX_ATTR).TryConvert2Int();
            var l = xe.GetAttribute(LENGTH_ATTR).TryConvert2Int();
            var a = xe.GetAttribute(ANAPHORIA_ATTR);

            if (i.HasValue && l.HasValue && !string.IsNullOrWhiteSpace(a))
                return (OffsetPairAnaphoria.Create(i.Value, l.Value, a));

            throw (new ArgumentException("Node not has all needed attributes"));
        }
        protected static OffsetPair ReadXmlOffsetPair(XDocument xd, string name, bool throwIfNull = false)
        {
            var xpath = ROOT_TAG + "/" + name;
            var xe = xd.XPathSelectElement(xpath);
            if (xe == null)
                return (null); //throw (new ArgumentException("Not found node on xpath: " + xpath ));

            var i = xe.GetAttribute(BEGININDEX_ATTR).TryConvert2Int();
            var l = xe.GetAttribute(LENGTH_ATTR).TryConvert2Int();

            if (i.HasValue && l.HasValue)
                return (OffsetPair.Create(i.Value, l.Value));
            if (!i.HasValue && !l.HasValue)
            {
                if (throwIfNull)
                    throw (new ArgumentException("Node not has needed attributes, xpath: " + xpath));
                return (null);
            }

            throw (new ArgumentException("Node not has all needed attributes, xpath: " + xpath));
        }

        protected static void WriteXmlOffsetPair(XmlWriter writer, OffsetPair[] offsetPair, string name)
        {
            if (offsetPair != null && offsetPair.Any())
            {
                writer.WriteStartElement(name);
                foreach (var op in offsetPair)
                {
                    WriteXmlOffsetPair(writer, op, ITEM_TAG);
                }
                writer.WriteEndElement();
            }
        }
        protected static void WriteXmlOffsetPair(XmlWriter writer, OffsetPair   offsetPair, string name)
        {
            if (offsetPair != null)
            {
                writer.WriteStartElement(name);
                writer.WriteAttributeString(BEGININDEX_ATTR, offsetPair.BeginIndex.ToString());
                writer.WriteAttributeString(LENGTH_ATTR, offsetPair.Length.ToString());
                writer.WriteEndElement();
            }
        }
        protected static void WriteXmlOffsetPair(XmlWriter writer, OffsetPairTonality[] offsetPair, string name)
        {
            if (offsetPair != null && offsetPair.Any())
            {
                writer.WriteStartElement(name);
                foreach (var op in offsetPair)
                {
                    WriteXmlOffsetPair(writer, op, ITEM_TAG);
                }
                writer.WriteEndElement();
            }
        }
        protected static void WriteXmlOffsetPair(XmlWriter writer, OffsetPairTonality   offsetPair, string name)
        {
            if (offsetPair != null)
            {
                writer.WriteStartElement(name);
                writer.WriteAttributeString(BEGININDEX_ATTR, offsetPair.BeginIndex.ToString());
                writer.WriteAttributeString(LENGTH_ATTR, offsetPair.Length.ToString());
                writer.WriteAttributeString(FRT_ATTR, offsetPair.FRT.ToString());
                writer.WriteEndElement();
            }
        }
        protected static void WriteXmlOffsetPair(XmlWriter writer, OffsetPairAnaphoria[] offsetPair, string name)
        {
            if (offsetPair != null && offsetPair.Any())
            {
                writer.WriteStartElement(name);
                foreach (var op in offsetPair)
                {
                    WriteXmlOffsetPair(writer, op, ITEM_TAG);
                }
                writer.WriteEndElement();
            }
        }
        protected static void WriteXmlOffsetPair(XmlWriter writer, OffsetPairAnaphoria   offsetPair, string name)
        {
            if (offsetPair != null)
            {
                writer.WriteStartElement(name);
                writer.WriteAttributeString(BEGININDEX_ATTR, offsetPair.BeginIndex.ToString());
                writer.WriteAttributeString(LENGTH_ATTR, offsetPair.Length.ToString());
                writer.WriteAttributeString(ANAPHORIA_ATTR, offsetPair.Value.ToString());
                writer.WriteEndElement();
            }
        }
    }

    /// <summary>
    /// 
    /// </summary>
    internal abstract class OffsetsTextSerializerBase
    {
        #region [.const's.]
        protected const string FIELD_SEPARATOR      = ";";
        protected const string ITEM_FIELD_SEPARATOR = ",";
        protected const string VALUE_SEPARATOR      = ":";
        #endregion

        protected OffsetsTextSerializerBase()
        {
        }

        protected static OffsetPair[] ReadOffsetPairs(string value)
        {
            var a = value.Split(new[] { ITEM_FIELD_SEPARATOR }, StringSplitOptions.RemoveEmptyEntries);
            if (!a.Any())
                return (null); //throw (new ArgumentException("Not found node on xpath: " + xpath ));

            var list = new List<OffsetPair>();
            foreach (var e in a)
            {
                list.Add(ReadOffsetPair(e));
            }
            if (!list.Any())
                throw (new ArgumentException("Node not has needed child elements, value: " + value));

            return (list.ToArray());
        }
        protected static OffsetPair   ReadOffsetPair(string value)
        {
            var a = value.Split(new[] { VALUE_SEPARATOR }, StringSplitOptions.RemoveEmptyEntries);
            if (a.Length != 2)
                return (null);

            var i = a[0].TryConvert2Int();
            var l = a[1].TryConvert2Int();

            if (i.HasValue && l.HasValue)
                return (OffsetPair.Create(i.Value, l.Value));

            throw (new ArgumentException("Node not has all needed text, value: " + value));
        }
        protected static OffsetPairTonality[] ReadOffsetPairTonalitys(string value)
        {
            var a = value.Split(new[] { ITEM_FIELD_SEPARATOR }, StringSplitOptions.RemoveEmptyEntries);
            if (!a.Any())
                return (null); //throw (new ArgumentException("Not found node on xpath: " + xpath ));

            var list = new List<OffsetPairTonality>();
            foreach (var e in a)
            {
                list.Add(ReadOffsetPairTonality(e));
            }
            if (!list.Any())
                throw (new ArgumentException("Node not has needed child elements, value: " + value));

            return (list.ToArray());
        }
        protected static OffsetPairTonality   ReadOffsetPairTonality(string value)
        {
            var a = value.Split(new[] { VALUE_SEPARATOR }, StringSplitOptions.RemoveEmptyEntries);
            if (a.Length != 3)
                return (null);

            var i = a[0].TryConvert2Int();
            var l = a[1].TryConvert2Int();
            var f = a[2].TryConvert2Int();

            if (i.HasValue && l.HasValue && f.HasValue)
                return (OffsetPairTonality.Create(i.Value, l.Value, f.Value));

            throw (new ArgumentException("Node not has all needed text, value: " + value));
        }
        protected static OffsetPairAnaphoria[] ReadOffsetPairAnaphorias(string value)
        {
            var a = value.Split(new[] { ITEM_FIELD_SEPARATOR }, StringSplitOptions.RemoveEmptyEntries);
            if (!a.Any())
                return (null); //throw (new ArgumentException("Not found node on xpath: " + xpath ));

            var list = new List<OffsetPairAnaphoria>();
            foreach (var e in a)
            {
                list.Add(ReadOffsetPairAnaphoria(e));
            }
            if (!list.Any())
                throw (new ArgumentException("Node not has needed child elements, value: " + value));

            return (list.ToArray());
        }
        protected static OffsetPairAnaphoria   ReadOffsetPairAnaphoria(string value)
        {
            var a = value.Split(new[] { VALUE_SEPARATOR }, StringSplitOptions.RemoveEmptyEntries);
            if (a.Length != 3)
                return (null);

            var i = a[0].TryConvert2Int();
            var l = a[1].TryConvert2Int();
            var a_ = a[2];

            if (i.HasValue && l.HasValue && !string.IsNullOrWhiteSpace(a_))
                return (OffsetPairAnaphoria.Create(i.Value, l.Value, a_));

            throw (new ArgumentException("Node not has all needed text, value: " + value));
        }
        protected static OffsetPair ReadOffsetPair(string value, bool throwIfNull = false)
        {
            var a = value.Split(new[] { VALUE_SEPARATOR }, StringSplitOptions.RemoveEmptyEntries);
            if (a.Length != 2)
                return (null); //throw (new ArgumentException("Not found node on xpath: " + xpath ));

            var i = a[0].TryConvert2Int();
            var l = a[1].TryConvert2Int();

            if (i.HasValue && l.HasValue)
                return (OffsetPair.Create(i.Value, l.Value));
            if (!i.HasValue && !l.HasValue)
            {
                if (throwIfNull)
                    throw (new ArgumentException("Node not has needed text, value: " + value));
                return (null);
            }

            throw (new ArgumentException("Node not has all needed text, xpath: " + value));
        }

        protected static void WriteOffsetPair(StringBuilder sb, OffsetPair[] offsetPair)
        {
            if (offsetPair != null && offsetPair.Any())
            {
                var index = 0;
                foreach (var op in offsetPair)
                {
                    WriteOffsetPair(sb, op);

                    if (index++ < offsetPair.Length - 1)
                    {
                        sb.Append(ITEM_FIELD_SEPARATOR);
                    }
                }
            }
        }
        protected static void WriteOffsetPair(StringBuilder sb, OffsetPair   offsetPair)
        {
            if (offsetPair != null)
            {
                sb.Append(offsetPair.BeginIndex.ToString() + VALUE_SEPARATOR +
                                        offsetPair.Length.ToString());
            }
        }
        protected static void WriteOffsetPair(StringBuilder sb, OffsetPairTonality[] offsetPair)
        {
            if (offsetPair != null && offsetPair.Any())
            {
                var index = 0;
                foreach (var op in offsetPair)
                {
                    WriteOffsetPair(sb, op);

                    if (index++ < offsetPair.Length - 1)
                    {
                        sb.Append(ITEM_FIELD_SEPARATOR);
                    }
                }
            }
        }
        protected static void WriteOffsetPair(StringBuilder sb, OffsetPairTonality   offsetPair)
        {
            if (offsetPair != null)
            {
                sb.Append(offsetPair.BeginIndex.ToString() + VALUE_SEPARATOR +
                                        offsetPair.Length.ToString() + VALUE_SEPARATOR +
                                        offsetPair.FRT.ToString());
            }
        }
        protected static void WriteOffsetPair(StringBuilder sb, OffsetPairAnaphoria[] offsetPair)
        {
            if (offsetPair != null && offsetPair.Any())
            {
                var index = 0;
                foreach (var op in offsetPair)
                {
                    WriteOffsetPair(sb, op);

                    if (index++ < offsetPair.Length - 1)
                    {
                        sb.Append(ITEM_FIELD_SEPARATOR);
                    }
                }
            }
        }
        protected static void WriteOffsetPair(StringBuilder sb, OffsetPairAnaphoria   offsetPair)
        {
            if (offsetPair != null)
            {
                sb.Append(offsetPair.BeginIndex.ToString() + VALUE_SEPARATOR +
                                        offsetPair.Length.ToString() + VALUE_SEPARATOR +
                                        offsetPair.Value.ToString());
            }
        }
    }
    //------------------------------------------------------------------//

    /// <summary>
    /// 
    /// </summary>
    public abstract class OffsetsProcessorBase< T > where T : OffsetsBase
    {
#if Create_NodeOffsetMap_Over_Mikl_Algorithm
        protected OffsetGenerator          _OffsetGenerator;
#endif
        protected Dictionary< XNode, int > _NodeOffsetMap;
        protected XElement                 _Sent;
        protected XNode[]                  __sentDescendantNodesAndSelf;
        protected XNode[]                  _SentDescendantNodesAndSelf
        {
            get
            {
                if ( __sentDescendantNodesAndSelf == null )
                {
                    __sentDescendantNodesAndSelf = _Sent.DescendantNodesAndSelf().ToArray();
                }
                return (__sentDescendantNodesAndSelf);
            }
        }
        protected int                      _SentPosition
        {
            get { return (_NodeOffsetMap.First().Value); }
        }        
#if Create_NodeOffsetMap_Over_Mikl_Algorithm
        public int                         MaxErrorPersent
        {
            get { return _OffsetGenerator.MaxErrorPersent; }
            set { _OffsetGenerator.MaxErrorPersent = value; }
        }
        public string                      LastError
        {
            get { return _OffsetGenerator.LastError; }
        }
#endif

#if Create_NodeOffsetMap_Over_Mikl_Algorithm
        public string  OriginalText
        {
            get;
            private set;
        }
        public OffsetsProcessorBase( string originalText )
        {
            originalText.ThrowIfEmptyOrNull("originalText");

            _OffsetGenerator = new OffsetGenerator( originalText );
            OriginalText = originalText;
        }
#else
        public OffsetsProcessorBase()
        {
        }
#endif

        public string CreateOffsetsString( XElement text, TextFormat textFormat = TextFormat.Text )
        {
            text.ThrowIfNull("text");

#if Create_NodeOffsetMap_Over_Mikl_Algorithm
            _NodeOffsetMap = Create_NodeOffsetMap_Over_Mikl_Algorithm( _OffsetGenerator, text );
#else
            _NodeOffsetMap = Create_NodeOffsetMap_Over_RDF_POS_Attributes( text );
#endif
            #region [.Prepare.]
            /*
            var termList = new List< string >();
            var textNodes = text.DescendantNodes()
                .Where( t => t.NodeType == XmlNodeType.CDATA )
                .Select( t =>
                {
                    var value = ((XCData) t).Value.Trim()
                                                  .Replace('ё', 'е')
                                                  .Replace('Ё', 'Е');
                    if (string.IsNullOrEmpty( value )) return (null);
                    var tList = OffsetGenerator.Tokenize( value );
                    if (!tList.Any()) return (null);
                    int startTermIndex = termList.Count;
                    termList.AddRange( tList );
                    return new 
                    { 
                        Node           = t, 
                        StartTermIndex = startTermIndex 
                    };
                }
            )
            .Where( t => t != null )
            .ToArray();

            if ( !termList.Any() )
            {
                throw (new ApplicationException("'termList' is empty"));
            }
            var offsetList = _OffsetGenerator.GetOffsetList( termList.ToArray() );
            _NodeOffsetMap = textNodes.ToDictionary( t => t.Node, t => offsetList[ t.StartTermIndex ] );
            */
            #endregion

            _Sent = text;
            var offsets = CreateOffsets();
            return (offsets.ToString( textFormat ));
        }

#if Create_NodeOffsetMap_Over_Mikl_Algorithm
        private static Dictionary< XNode, int > Create_NodeOffsetMap_Over_Mikl_Algorithm( OffsetGenerator offsetGenerator, XElement text )
        {
            var termList = new List< string >();
            var textNodes = text.DescendantNodes()
                .Where( t => t.NodeType == XmlNodeType.CDATA )
                .Select( t =>
                {
                    var value = ((XCData) t).Value.Trim()
                                                  .Replace('ё', 'е')
                                                  .Replace('Ё', 'Е');
                    if (string.IsNullOrEmpty( value )) return (null);
                    var tList = OffsetGenerator.Tokenize( value );
                    if (!tList.Any()) return (null);
                    int startTermIndex = termList.Count;
                    termList.AddRange( tList );
                    return new 
                    { 
                        Node           = t, 
                        StartTermIndex = startTermIndex 
                    };
                }
            )
            .Where( t => t != null )
            .ToArray();

            if ( !termList.Any() )
            {
                throw (new ApplicationException("'termList' is empty"));
            }
            var offsetList = offsetGenerator.GetOffsetList( termList.ToArray() );
            var nodeOffsetMap = textNodes.ToDictionary( t => t.Node, t => offsetList[ t.StartTermIndex ] );

            return (nodeOffsetMap);
        }
#endif
        private static Dictionary< XNode, int > Create_NodeOffsetMap_Over_RDF_POS_Attributes( XElement text )
        {
            var sents = text.GetSents().ToArray();
            if ( sents.Length == 0 )
            {
                sents = new[] { text };
            }
            var nm = new Dictionary< XNode, int >();
              //nm.Add( cdatas[ 0 ], 0 );

            foreach ( var sent in sents )
            {
                var sent_pos = sent.GetForceAttributePOS();

                var cdatas = sent.DescendantNodesCDATA()
                                 //.Where( n => !string.IsNullOrWhiteSpace( n.Value ) )
                                 //.Where( n => !string.IsNullOrEmpty( n.Value ) )
                                 .Where( (n, i) => (i == 0) || !string.IsNullOrWhiteSpace( n.Value ) )
                                 .ToArray();

                nm.Add( cdatas[ 0 ], sent_pos );
                for ( var i = 1; i < cdatas.Length; i++ )
                {
                    var cdata = cdatas[ i ];
                    var p = cdata.Parent.Parent;

                    if ( p.HasMoreThanOneChild() )
                    {
                        var prev = cdatas[ i - 1 ];
                        var pos = nm[ prev ] + prev.Value.Length + (cdata.Value.Length - cdata.Value.TrimStart().Length);

                        nm.Add( cdata, pos );
                    }
                    else
                    {
                        var pos = p.GetAttributePOS();
                        if ( pos.HasValue )
                        {
                            nm.Add( cdata, sent_pos + pos.Value );
                        }
                        else
                        {
                            var prev = cdatas[ i - 1 ];
                            var pos2 = nm[ prev ] + prev.Value.Length + (cdata.Value.Length - cdata.Value.TrimStart().Length);

                            nm.Add( cdata, pos2 );
                        }
                    }
                }
            }

            return (nm);
        }

        protected abstract T CreateOffsets();

        protected OffsetPair Get4Sentence()
        {
            var last = _NodeOffsetMap.Last();
            return (OffsetPair.Create( _SentPosition, last.Value - _SentPosition + ((XCData) last.Key).Value.Trim().Length ));
        }
        protected OffsetPair Get4ObjectSubject( XElement element )
        {
            if ( element.IsNotNull() )
            {
                return (getOffsetPair4Element( element ));
            }
            return (null);
        }
        protected Tuple< OffsetPairTonality[], OffsetPairTonality[], OffsetPair[] > Get4Tonality()
        {
            #region [.positive & negative.]
            var t = from d in _Sent.Descendants()
                            let sntfinal = d.GetAttributeSNTFINAL()
                            where sntfinal.IsNotNull()
                            let frtfinal = d.GetAttributeFRTFINAL()
                            select new
                            {
                                element = d,
                                snt = sntfinal,
                                frt = frtfinal
                            };

            var positive = new Lazy< List< OffsetPairTonality > >( LazyThreadSafetyMode.None );
            var negative = new Lazy< List< OffsetPairTonality > >( LazyThreadSafetyMode.None );
            foreach (var a in t)
            {
                var op = getOffsetPair4Element(a.element);
                if (op.IsNull())
                    continue;

                var list = a.snt.ToLower().Contains("neg")
                        ? negative.Value
                        : positive.Value;

                list.Add(OffsetPairTonality.Create( op, a.frt ));
            }

            var positiveTonality = positive.IsValueCreated ? positive.Value.Distinct( OffsetPairTonality.Comparer ).ToArray() : null;
            var negativeTonality = negative.IsValueCreated ? negative.Value.Distinct( OffsetPairTonality.Comparer ).ToArray() : null;
            #endregion

            #region [.neutral.]
            var neutralTonality = default(OffsetPair[]);
            if (positiveTonality.IsNotNull() || negativeTonality.IsNotNull())
            {
                var neutral = new Lazy< List< OffsetPair > >( LazyThreadSafetyMode.None );

                var subsents = from s in _Sent.Descendants()
                               where s.IsElementSUBSENT()
                               let sntfinal = s.GetAttributeSNTFINAL()
                               where sntfinal.IsNull()
                               select s;
                foreach ( var subsent in subsents )
                {
                    var op = getOffsetPair4Element( subsent );
                    if ( op.IsNull() )
                        continue;

                    neutral.Value.Add( op );
                }

                #region commented
                /*
                if ( neutral.IsValueCreated )
                {
                    neutralTonality = neutral.Value.ToArray();
                }
                */
                #endregion

                #region [.except unused parts.]
                if ( neutral.IsValueCreated )
                {
                    neutralTonality = neutral.Value.ToArray();

                    var tonality = positiveTonality.ConcatEx( negativeTonality );
                    if ( tonality.IsNotNull() )
                    {
                        neutralTonality = neutralTonality.Except
                                                         (
                                                            neutralTonality.Except( tonality, OffsetPair.Comparer )
                                                         )
                                                         .Distinct( OffsetPairTonality.Comparer )
                                                         .ToArray();
                    }
                }
                #endregion
            }
            #endregion

            #region [.result.]
            return (Tuple.Create( positiveTonality, negativeTonality, neutralTonality ));
            #endregion
        }
        protected OffsetPairAnaphoria[] Get4Anaphoria()
        {
            var t = from d in _Sent.DescendantsEntityOrCet()
                            let   link = d.GetAttributeLINK()
                            where link.HasValue
                            let   fullname = d.GetAttributeFULLNAME()
                            select new
                            {
                                element  = d,
                                fullname = fullname
                            };

            var lazylist = new Lazy< List< OffsetPairAnaphoria > >( LazyThreadSafetyMode.None );
            foreach ( var a in t )
            {
                var op = getOffsetPair4Element( a.element );
                if ( op.IsNull() )
                    continue;

                lazylist.Value.Add( OffsetPairAnaphoria.Create( op, a.fullname ) );
            }

            return (lazylist.IsValueCreated ? lazylist.Value.ToArray() : null);
        }

        protected OffsetPair getOffsetPair4Element( XElement element )
        {
            var cdatas = element.DescendantsNotNullOrWhiteSpaceXCData().ToArray();
            if ( !cdatas.Any() )
                return (null);

            var pos = _NodeOffsetMap.TryGetValueEx( cdatas.First() );
            if ( !pos.HasValue )
                return (null);

            var last = cdatas.Last();
            var last_pos = _NodeOffsetMap.TryGetValueEx( last );
            if ( !last_pos.HasValue )
                return (null);

            var len = (last_pos.Value - pos.Value) + last.Value.Trim().Length;
            if ( len <= 0 )
                return (null);

            pos -= _SentPosition;
            /*if ( pos.Value < 0 )
                return (null);
            */

            return (OffsetPair.Create( pos.Value, len ));
        }
    }

    /// <summary>
    /// 
    /// </summary>
    internal static class OffsetsString2XDocumentConverterHelper
    {
        #region [.const's.]
        private const string TEXT_TAG           = "<text/>";
        private const string TEXT               = "text";
        private const string SUBJECT            = "subject";
        private const string OBJECT             = "object";
        private const string INDIRECT_SPEECH    = "indirect-speech";
        private const string DIRECT_SPEECH      = "direct-speech";
        private const string POSITIVE_TONALITY  = "positive-tonality";
        private const string NEGATIVE_TONALITY  = "negative-tonality";
        private const string NEUTRAL_TONALITY   = "neutral-tonality";
        private const string ANAPHORIA          = "anaphoria";

        private const string FRT_ATTRIBUTE       = "frt";
        private const string ANAPHORIA_ATTRIBUTE = "anaphoria";
        #endregion

        public static XDocument ConvertToFlat< T >( string originalText, T offsets, IEnumerable< point > points )
            where T : OffsetsBase
        {
            originalText .ThrowIfEmptyOrNull("originalText" );
            offsets      .ThrowIfNull       ("offsets"      );            

            var sent = originalText.SubstringEx( offsets.Sentence.BeginIndex, offsets.Sentence.Length );

            var xdoc = XDocument.Parse( TEXT_TAG );

            var opts = points.OrderBy( pt => pt, point.Comparer ).ToArray();
            if ( opts.Any() )
            {
                var startPoint = point.empty();
                foreach ( var pt in opts )
                {
                    if ( sent.Length < startPoint.value ) //foolproof, lingustic-glitch's!!!
                        break;

                    pt.InsertMarker( xdoc.Root, startPoint.value, sent );
                    startPoint = pt;
                }

                var last_pos = opts.Last().value;
                if ( last_pos < offsets.Sentence.Length )
                {
                    xdoc.Root.Add( sent.SubstringEx( last_pos, offsets.Sentence.Length - last_pos ) );
                }
            }
            else
            {
                xdoc.Root.Add( sent );
            }

            return (xdoc);
        }

        public static XDocument ConvertToHierarchy< T >( string originalText, T offsets, IEnumerable< point > points )
            where T : OffsetsBase
        {
            originalText .ThrowIfEmptyOrNull("originalText" );
            offsets      .ThrowIfNull       ("offsets"      );            

            var sent = originalText.SubstringEx( offsets.Sentence.BeginIndex, offsets.Sentence.Length );

            var xdoc = default(XDocument);

            var opts = points.OrderBy( pt => pt, point.Comparer ).ToArray();
            if ( opts.Any() )
            {
                var sb = new StringBuilder();
                using ( var writer = XmlWriter.Create( sb ) )
                {
                    writer.WriteStartElement( TEXT );
                    var startPoint = point.empty();
                    foreach ( var pt in opts )
                    {
                        if ( sent.Length < startPoint.value ) //foolproof, lingustic-glitch's!!!
                            break;

                        pt.InsertMarker( writer, startPoint.value, sent );
                        startPoint = pt;
                    }

                    var last_pos = opts.Last().value;
                    if ( last_pos < offsets.Sentence.Length )
                    {
                        writer.WriteString( sent.SubstringEx( last_pos, offsets.Sentence.Length - last_pos ) );
                    }
                    writer.WriteEndElement();
                }
                xdoc = XDocument.Parse( sb.ToString() );
            }
            else
            {
                xdoc = XDocument.Parse( TEXT_TAG );
                xdoc.Root.Add( sent );
            }

            return (xdoc);
        }

        internal enum point_type
        {
            start,
            end
        }
        internal enum tag_type
        {
            anaphoria,
            subject,
            @object,
            indirectspeech,
            directspeech,
            positivetonality,
            negativetonality,
            neutraltonality,
        }
        internal class point : IComparer< point >
        {
            public int        value
            {
                get;
                set;
            }
            public point_type type
            {
                get;
                set;
            }
            public tag_type   tag
            {
                get;
                set;
            }
            public int?       frt
            {
                get;
                set;
            }
            public string     anaphoria
            {
                get;
                set;
            }

            public point head
            {
                get;
                set;
            }

            public override string ToString()
            {
                return (value + " - " + type + ", " + tag);
            }

            public static point empty()
            {
                return (new point());
            }
            public static point[] createFrom( OffsetPair          offsetPair, tag_type tag_ )
            {
                if ( offsetPair != null )
                {                    
                    var s = new point() { value = offsetPair.BeginIndex                    , type = point_type.start, tag = tag_ };
                    var e = new point() { value = offsetPair.BeginIndex + offsetPair.Length, type = point_type.end  , tag = tag_, head = s };
                    
                    return (new[] { s, e });
                }
                return (new point[ 0 ]);
            }
            public static point[] createFrom( OffsetPairTonality  offsetPair, tag_type tag_ )
            {
                if ( offsetPair != null )
                {
                    var s = new point() { value = offsetPair.BeginIndex                    , type = point_type.start, tag = tag_, frt = offsetPair.FRT };
                    var e = new point() { value = offsetPair.BeginIndex + offsetPair.Length, type = point_type.end  , tag = tag_, head = s };

                    return (new[] { s, e });
                }
                return (new point[ 0 ]);
            }
            public static point[] createFrom( OffsetPairAnaphoria offsetPair, tag_type tag_ )
            {
                if ( offsetPair != null )
                {
                    var s = new point() { value = offsetPair.BeginIndex                    , type = point_type.start, tag = tag_, anaphoria = offsetPair.Value };
                    var e = new point() { value = offsetPair.BeginIndex + offsetPair.Length, type = point_type.end  , tag = tag_, head = s };

                    return (new[] { s, e });
                }
                return (new point[ 0 ]);
            }
            public static point[] createFrom( OffsetPair[]          offsetPairs, tag_type tag_ )
            {
                if ( offsetPairs != null )
                {
                    var list = new List< point >();
                    foreach ( var op in offsetPairs )
                    {
                        list.AddRange( createFrom( op, tag_ ) );
                    }
                    return (list.ToArray());
                }
                return (new point[ 0 ]);
            }
            public static point[] createFrom( OffsetPairTonality[]  offsetPairs, tag_type tag_ )
            {
                if ( offsetPairs != null )
                {
                    var list = new List< point >();
                    foreach ( var op in offsetPairs )
                    {
                        list.AddRange( createFrom( op, tag_ ) );
                    }
                    return (list.ToArray());
                }
                return (new point[ 0 ]);
            }
            public static point[] createFrom( OffsetPairAnaphoria[] offsetPairs, tag_type tag_ )
            {
                if ( offsetPairs != null )
                {
                    var list = new List< point >();
                    foreach ( var op in offsetPairs )
                    {
                        list.AddRange( createFrom( op, tag_ ) );
                    }
                    return (list.ToArray());
                }
                return (new point[ 0 ]);
            }

            private static string tag2tagName( tag_type tag )
            {
                switch ( tag )
                {
                    case tag_type.subject:          return (SUBJECT);
                    case tag_type.@object:          return (OBJECT);
                    case tag_type.indirectspeech:   return (INDIRECT_SPEECH);
                    case tag_type.directspeech:     return (DIRECT_SPEECH);
                    case tag_type.positivetonality: return (POSITIVE_TONALITY);
                    case tag_type.negativetonality: return (NEGATIVE_TONALITY);
                    case tag_type.neutraltonality:  return (NEUTRAL_TONALITY);
                    case tag_type.anaphoria:        return (ANAPHORIA);
                    default:
                        throw (new ArgumentException(tag.ToString()));
                }
            }

            public void InsertMarker( XElement element, int startIndex, string text )
            {
                const string BEGIN_POSTFIX = "-begin";
                const string END_POSTFIX   = "-end";

                #region [.create xelement.]
                var marker  = tag2tagName( tag ) + ((type == point_type.start) ? BEGIN_POSTFIX : END_POSTFIX);
                var xmarker = new XElement( marker );
                if ( frt.HasValue )
                {
                    xmarker.Add( new XAttribute( FRT_ATTRIBUTE, frt.Value ) );
                }
                if ( anaphoria.IsNotNull() )
                {
                    xmarker.Add( new XAttribute( ANAPHORIA_ATTRIBUTE, anaphoria ) );
                }
                #endregion

                element.Add( text.SubstringEx( startIndex, value - startIndex ) );
                element.Add( xmarker );
            }
            public void InsertMarker( XmlWriter writer, int startIndex, string text )
            {
                writer.WriteString( text.SubstringEx( startIndex, value - startIndex ) );

                if ( type == point_type.start )
                {
                    var marker = tag2tagName( tag );
                    writer.WriteStartElement( marker );
                    if ( frt.HasValue )
                    {
                        writer.WriteAttributeString( FRT_ATTRIBUTE, frt.Value.ToString() );
                    }
                    if ( anaphoria.IsNotNull() )
                    {
                        writer.WriteAttributeString( ANAPHORIA_ATTRIBUTE, anaphoria );
                    }                    
                }
                else
                {
                    writer.WriteEndElement();
                }
            }

            #region [.IComparer< point >.]
            public static readonly point Comparer = new point();

            public int Compare( point x, point y )
            {
                var c = x.value - y.value;
                if (c != 0)
                    return (c);

                c = y.type - x.type;
                if (c != 0)
                    return (c);

                if ( y.type == point_type.end )
                {
                    c = Compare( x.head, y.head );
                    if (c != 0)
                        return (-c);

                    return (CompareByTagName( y.tag, x.tag ));
                }
                
                return (CompareByTagName( x.tag, y.tag ));
            }

            private static bool InGroup1( tag_type tag )
            {
                switch (tag)
                {
                    case tag_type.subject:
                    case tag_type.@object:
                    case tag_type.anaphoria:
                    case tag_type.neutraltonality:
                        return (true);
                }
                return (false);
            }
            private static bool InGroup2( tag_type tag )
            {
                return (!InGroup1(tag));
            }
            private static bool InGroup3( tag_type tag )
            {
                switch ( tag )
                {
                    case tag_type.positivetonality:
                    case tag_type.negativetonality:
                        return (true);
                }
                return (false);
            }
            private static bool InGroup4( tag_type tag )
            {
                return (!InGroup3(tag));
            }
            /*private static int GetWeight( tag_type tag )
            {
                switch (tag)
                {
                    case tag_type.subject:
                    case tag_type.@object:
                    case tag_type.anaphoria:
                    case tag_type.neutraltonality:
                        return (1);
                    case tag_type.positivetonality:
                    case tag_type.negativetonality:
                        return (2);
                }
                return (0);
            }*/
            private static int CompareByTagName( tag_type x_tag, tag_type y_tag )
            {
                if ( InGroup1( x_tag ) && InGroup2( y_tag ) )
                    return (1);

                if ( InGroup1( y_tag ) && InGroup2( x_tag ) )
                    return (-1);


                if ( InGroup3( x_tag ) && InGroup4( y_tag ) )
                    return (1);

                if ( InGroup3( y_tag ) && InGroup4( x_tag ) )
                    return (-1);


                return (((int) y_tag) - ((int) x_tag));
            }
            #endregion
        }
    }
    //------------------------------------------------------------------//

    /// <summary>
    /// 
    /// </summary>
    internal static class Extensions4Offsets
    {
        #region [.consts.]
        public const string DIRECTSPEECH_BEGIN        = "DIRECTSPEECH-BEGIN";
        public const string DIRECTSPEECH_END          = "DIRECTSPEECH-END";
        public const string INDIRECTSPEECH_BEGIN      = "INDIRECTSPEECH-BEGIN";
        public const string INDIRECTSPEECH_END        = "INDIRECTSPEECH-END";
        private const string SNTFINAL_ATTRIBUTE       = "SNTFINAL";
        private const string FRTFINAL_ATTRIBUTE       = "FRTFINAL";
        private const string LINK_ATTRIBUTE           = "LINK";
        private const string FULLNAME_ATTRIBUTE       = "FULLNAME";
        private const string INQUIRYINITIAL_ATTRIBUTE = "INQUIRYINITIAL";
        private const string ISSUBJECT_ATTRIBUTE      = "ISSUBJECT";
        private const string POS_ATTRIBUTE            = "POS";
        private const string ENTITY_ELEMENT           = "ENTITY";
        private const string CET_ELEMENT              = "CET";
        private const string SENT_ELEMENT             = "SENT";
        private const string U_ELEMENT                = "U";
        private const string T_ELEMENT                = "T";
        private const string SUBSENT_ELEMENT          = "SUB_SENT";
        #endregion

        public static bool IsNull(this object obj)
        {
            return (obj == null);
        }
        public static bool IsNotNull(this object obj)
        {
            return (obj != null);
        }
        public static int? TryConvert2Int(this string value)
        {
            int v;
            if (int.TryParse(value, out v))
                return (v);
            return (null);
        }
        public static int? TryGetValueEx(this Dictionary<XNode, int> nodeOffsetMap, XNode node)
        {
            var value = 0;
            if (nodeOffsetMap.TryGetValue(node, out value))
                return (value);
            return (null);
        }
        public static string ToStartTag(this string value)
        {
            return ("<" + value + ">");
        }
        public static string ToEndTag(this string value)
        {
            return ("</" + value + ">");
        }
        public static void ThrowIfNull(this object obj, string argName)
        {
            if (obj == null)
                throw (new ArgumentNullException(argName));
        }
        public static void ThrowIfEmptyOrNull(this string text, string argName)
        {
            if (string.IsNullOrEmpty(text))
                throw (new ArgumentNullException(argName));
        }
        public static IEnumerable<T> ConcatEx<T>(this IEnumerable<T> t1, IEnumerable<T> t2)
        {
            if (t1.IsNotNull())
            {
                return (t2.IsNotNull() ? t1.Concat(t2) : t1);
            }
            else if (t2.IsNotNull())
            {
                return (t2);
            }
            return (null);
        }
        public static string SubstringEx(this string value, int startIndex, int length)
        {
            var start = Math.Min( Math.Abs( startIndex ), value.Length );
            var len   = Math.Min( Math.Abs( length )    , Math.Abs( value.Length - startIndex ) );
                        //Math.Max( Math.Min( length   ,  value.Length - startIndex  ), 0 );
                        
            return (value.Substring( start, len ));

            //return (value.Substring(startIndex, Math.Min(length, value.Length - startIndex)));
        }

        public static IEnumerable< XCData > DescendantsNotNullOrWhiteSpaceXCData( this XElement element )
        {
            var descendantXCData = from d in element.DescendantNodes()
                                   where (d.NodeType == XmlNodeType.CDATA)
                                   let cdata = ((XCData) d)
                                   where ( !string.IsNullOrWhiteSpace( cdata.Value ) )
                                   select cdata;
            return (descendantXCData);
        }
        public static string GetAttribute(this XElement element, string name)
        {
            var a = element.Attribute(name);
            if (a != null)
                return (a.Value);

            return (null);
        }
        public static string GetAttributeSNTFINAL(this XElement element)
        {
            return (element.GetAttribute(SNTFINAL_ATTRIBUTE));
        }
        public static int GetAttributeFRTFINAL(this XElement element)
        {
            var value = element.GetAttribute(FRTFINAL_ATTRIBUTE).TryConvert2Int();
            return (value.GetValueOrDefault(0));
        }
        public static int? GetAttributeLINK(this XElement element)
        {
            return (element.GetAttribute(LINK_ATTRIBUTE).TryConvert2Int());
        }
        public static string GetAttributeFULLNAME(this XElement element)
        {
            return (element.GetAttribute(FULLNAME_ATTRIBUTE));
        }
        public static int? GetAttributePOS(this XElement element)
        {
            return (element.GetAttribute( POS_ATTRIBUTE ).TryConvert2Int());
        }
        public static bool IsINQUIRYINITIAL(this XElement element)
        {
            return (element.GetAttribute(INQUIRYINITIAL_ATTRIBUTE) == bool.TrueString.ToUpper());
        }
        public static bool ISSUBJECT(this XElement element)
        {
            return (element.GetAttribute(ISSUBJECT_ATTRIBUTE).IsNotNull()
                             &&
                         !element.IsINQUIRYINITIAL()
                             &&
                            element.XPathSelectElement("ancestor::SENT//*[ @ISSUBJECT and @INQUIRYINITIAL ]").IsNull());
        }
        public static IEnumerable<XElement> DescendantsEntity(this XElement element)
        {
            return (element.Descendants(ENTITY_ELEMENT));
        }
        public static IEnumerable<XElement> DescendantsEntityOrCet(this XElement element)
        {
            return (element.Descendants(ENTITY_ELEMENT)
                                    .Concat
                                    (
                                    element.Descendants(CET_ELEMENT)
                                    )
                            );
        }
        public static XElement ElementT(this XElement element)
        {
            return (element.Element(T_ELEMENT));
        }
        public static void ThrowIfNotSent(this XElement sent)
        {
            if (sent.Name != SENT_ELEMENT)
                throw (new ArgumentException("sent.Name != SENT_ELEMENT"));
        }
        public static bool IsElementSUBSENT(this XElement element)
        {
            return (element.Name == SUBSENT_ELEMENT);
        }
        public static bool IsElementENTITY(this XElement element)
        {
            return (element.Name == ENTITY_ELEMENT);
        }
        public static IEnumerable< XElement > DescendantsAndSelfWithExistsAttribute( this XElement element, string attributeName )
        {
            return (from d in element.DescendantsAndSelf()
                    where d.GetAttribute( attributeName ).IsNotNull()
                    select d
                   );
        }
        public static IEnumerable< T > RepeatOne< T >( this T t ) where T : class
        {
            if ( t != default(T) )
                return (Enumerable.Repeat< T >( t, 1 ));
            return (new T[ 0 ]);
        }
        public static IEnumerable< T > RepeatOne< T >( this T[] t ) where T : class
        {
            if ( t != default(T[]) )
                return (t);
            return (new T[ 0 ]);
        }

        public static IEnumerable< XElement > GetSents( this XElement e )
        {
            var r = e.Elements( SENT_ELEMENT );
            return (r);
        }
        public static IEnumerable< XCData > DescendantNodesCDATA( this XElement e )
        {
            var r = e.DescendantNodes()
                      .Where( n => n.NodeType == XmlNodeType.CDATA )
                      .Select( n => (XCData) n );
            return (r);
        }
        public static bool HasMoreThanOneChild( this XElement e )
        {
            using ( var it = e.Elements().GetEnumerator() )
            {
                if ( !it.MoveNext() )
                    return (false);
                if ( !it.MoveNext() )
                    return (false);
            }
            return (true);

            //foreach ( var c in e.Elements() )
            //{
                
            //}
        }
        public static int GetForceAttributePOS( this XElement e )
        {
            var a = e.Attribute( POS_ATTRIBUTE );
            if ( a == null )
                throw (new InvalidOperationException());
            var pos = int.Parse( a.Value );

            return (pos);
        }
        /*public static int? GetAttributePOS( this XElement e )
        {
            var a = e.Attribute( POS_ATTRIBUTE );
            if ( a == null )
                return (null);
            var pos = int.Parse( a.Value );

            return (pos);
        }*/
    }
}
