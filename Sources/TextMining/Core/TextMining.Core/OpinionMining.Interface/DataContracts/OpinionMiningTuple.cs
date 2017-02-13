using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Linq;
using System.Runtime.Serialization;
using System.Xml.Linq;

using TextMining.Core;
using Lingvistics;
using Linguistics.Coreference;

#if WITH_OM_TM
using BlockAttribute = Lingvistics.BlockAttribute;
using EntityRole     = Lingvistics.EntityRole;
using EntityType     = Lingvistics.EntityType;
using NodeName       = Lingvistics.NodeName;
#else
using BlockAttribute = Linguistics.Core.BlockAttribute;
using EntityRole     = Linguistics.Core.EntityRole;
using EntityType     = Linguistics.Core.EntityType;
using NodeName       = Linguistics.Core.UnitTextType;
#endif

namespace OpinionMining
{
    /// <summary>
    /// 
    /// </summary>
    [DataContract]
    [Serializable]
    public class EssenceItem
    {
        public EssenceItem( string value, string prof, TypeAttributeValue type )
        {
            value.ThrowIfEmptyOrNull("value");

            Value = EntityNormalizer.Normalize( value );
            Prof  = !prof.IsEmptyOrNull() ? EntityNormalizer.Normalize( prof ) : null;
            Type  = type.IsLinguisticsCompability()
					? type.ToString().ToEnum< EntityType >()
					: EntityType.Unknown; 
        }
        internal EssenceItem( XElement essenceElement )
        {
            var type = essenceElement.GetAttributeTYPESAVEDorTYPE(); //.GetAttributeTYPE();
            if ( !type.HasValue ) throw (new ArgumentNullException("type"));

            var value = essenceElement.GetVALUETHEMEorVALUE();
            value.ThrowIfEmptyOrNull("value");

            var prof = essenceElement.GetAttributePROF();

            Value = EntityNormalizer.Normalize( value );
            Prof  = !prof.IsEmptyOrNull() ? EntityNormalizer.Normalize( prof ) : null;
            Type  = type.Value.IsLinguisticsCompability()
					? type.Value.ToString().ToEnum< EntityType >()
					: EntityType.Unknown; 
        }

        [DataMember]
        public string Value
        {
            get;
            private set;
        }

        [DataMember]
        public string Prof
        {
            get;
            private set;
        }

        [DataMember]
		public EntityType Type
        {
            get;
            private set;
        }

        internal void SetValue( string newValue )
        {
            Value = newValue;
        }

        public override string ToString()
        {
            return (/*Prof ?? */Value);
        }
        public override bool Equals(object obj)
        {
            var e = obj as EssenceItem;
            if (e != null)
            {
                return ((Value == e.Value) && (Prof == e.Prof) && (Type == e.Type));
            }
            return base.Equals(obj);
        }
        public override int GetHashCode()
        {
            if (Prof != null)
                return (Value.GetHashCode() ^ Prof.GetHashCode() ^ Type.GetHashCode());
            return (Value.GetHashCode() ^ Type.GetHashCode());
        }
    }

    /// <summary>
    /// 
    /// </summary>
    [DataContract]
    [Serializable]
    public abstract class Essence
    {
        protected Essence()
        {
        }
        public Essence( SubjectAndAnaphoriaEntityTuple tuple )
        {
            tuple.ThrowIfNull("tuple");

            AnaphoriaAndEntityTuple = tuple;

            var essenceElement = tuple.Anaphoria ?? tuple.Entity;
            essenceElement.ThrowIfNull("essenceElement");

            var lst = new List< EssenceItem >( 1 );
            lst.Add( new EssenceItem( essenceElement ) );
            EssenceItems = lst.AsReadOnly();
        }

        [DataMember]
        public ReadOnlyCollection< EssenceItem > EssenceItems
        {
            get;
            private set;
        }

        public void SetEssenceItems4Homogeneous( ReadOnlyCollection< EssenceItem > essenceItems )
        {
            essenceItems.ThrowIfNull("essenceItems");
            if ( essenceItems.Count == 0 ) throw (new ArgumentException("essenceItems.Count == 0"));
            
            if ( 1 < EssenceItems.Count ) throw (new ArgumentException("1 < EssenceItems.Count"));

            EssenceItems = essenceItems;
        }

        public SubjectAndAnaphoriaEntityTuple AnaphoriaAndEntityTuple
        {
            get;
            private set;
        }
        public XElement Entity
        {
            get { return ((AnaphoriaAndEntityTuple != null) ? AnaphoriaAndEntityTuple.Entity : null); }
        }

        internal void TrySetValueWithCoreferenceInfo( ICoreferenceInfo coreferenceInfo )
        {
            if ( Entity != null )
            {
                var tuple = coreferenceInfo.TryGetMaster( Entity );
            
                TrySetValueWithCoreferenceInfo( tuple );
            }
        }
        internal void TrySetValueWithCoreferenceInfo( Tuple< string, XElement > tuple )
        {
            if ( tuple != null && tuple.Item1 != null )
            {
                var masterName = EntityNormalizer.Normalize( tuple.Item1 );

                foreach ( var essenceItem in EssenceItems )
                {
                    essenceItem.SetValue( masterName );
                }
            }
        }        

        public override string ToString()
        {
            return (EssenceItems.Select( ei => ei.ToString()).JoinBy("; "));
        }
        public override bool Equals(object obj)
        {
            var e = obj as Essence;
            if ( e != null )
            {
                if ( EssenceItems.Count != e.EssenceItems.Count )
                {
                    return (false);
                }

                return (EssenceItems.SequenceEqual( e.EssenceItems ));
            }
            return base.Equals(obj);
        }
        public override int GetHashCode()
        {
            return (EssenceItems.Aggregate( 0, (h, ei) => h ^ ei.GetHashCode() ));
        }
    }

    /// <summary>
    /// 
    /// </summary>
    [DataContract]
    [Serializable]
    public class ObjectEssence : Essence
    {        
        public ObjectEssence( SubjectAndAnaphoriaEntityTuple tuple, bool isSubjectIndeed = false ) 
            : base( tuple )
        {
            IsSubjectIndeed = isSubjectIndeed;
        }
        public ObjectEssence( XElement entity, bool isSubjectIndeed = false ) 
            : this( new SubjectAndAnaphoriaEntityTuple( entity ), isSubjectIndeed )
        {
        }

        [DataMember]
        public bool IsSubjectIndeed
        {
            get;
            private set;
        }

        public override bool Equals(object obj)
        {
            var o = obj as ObjectEssence;
            if ( o != null )
            {
                return (/*(Entity == o.Entity) &&*/ (IsSubjectIndeed == o.IsSubjectIndeed) && base.Equals( obj ) );
            }
            return base.Equals( obj );
        }
        public override int GetHashCode()
        {
            return base.GetHashCode();
        }
    }

    /// <summary>
    /// 
    /// </summary>
    [Serializable]
    public class SubjectAndAnaphoriaEntityTuple
    {
        public SubjectAndAnaphoriaEntityTuple( XElement subjectEntity, XElement anaphoriaLinkedEntity = null )
        {
            subjectEntity.ThrowIfNull("subjectEntity");

            Entity    = subjectEntity;
            Anaphoria = anaphoriaLinkedEntity;
        }

        [NonSerialized]
        private XElement _Entity;
        public XElement Entity
        {
            get { return (_Entity); }
            private set { _Entity = value; }
        }

        [NonSerialized]
        private XElement _Anaphoria;
        public XElement Anaphoria
        {
            get { return (_Anaphoria); }
            private set { _Anaphoria = value; }
        }

        public override bool Equals(object obj)
        {
            var e = obj as SubjectAndAnaphoriaEntityTuple;
            if ( e != null )
            {
                return (Entity == e.Entity && Anaphoria == e.Anaphoria);
            }
            return base.Equals(obj);        
        }
        public override int GetHashCode()
        {
            return (Entity.GetHashCode() ^ ((Anaphoria != null) ? Anaphoria.GetHashCode() : int.MaxValue));
        }
        public override string ToString()
        {
            return (Entity.Value + ((Anaphoria != null) ? (" -> " + Anaphoria.Value) : string.Empty));
        }
    }

    /// <summary>
    /// 
    /// </summary>
    [DataContract]
    [Serializable]
    public class SubjectEssence : Essence
    {
        public static SubjectEssence Author
        {
            get;
            private set;
        }
        static SubjectEssence()
        {
            Author = new SubjectEssence();
        }

        private SubjectEssence()
        {
            //SubjectEssences = new List< SubjectEssence >().AsReadOnly();
            IsAuthor = true;
        }
        public SubjectEssence( SubjectAndAnaphoriaEntityTuple tuple ) : base( tuple )
        {            
            Verb      = null;
            VerbValue = null;
        }
        public SubjectEssence( SubjectAndAnaphoriaEntityTuple tuple, XElement verb ) : base( tuple )
        {
            verb.ThrowIfNull("verb");

            Verb = verb;

            Initialize( verb.GetVALUE() );
        }
        public SubjectEssence( SubjectAndAnaphoriaEntityTuple tuple, string verbValue ) : base( tuple )
        {
            Verb = null;

            Initialize( verbValue );
        }
        public SubjectEssence( SubjectEssence subjectEssence, string verbValue ) : base( subjectEssence.AnaphoriaAndEntityTuple )
        {
            Verb = null;

            Initialize( verbValue );
        }
        private void Initialize( string verbValue )
        {
            verbValue.ThrowIfEmptyOrNull("verbValue");

            VerbValue = EntityNormalizer.Normalize( verbValue );
        }

        [DataMember]
        public string VerbValue
        {
            get;
            private set;
        }

        [DataMember]
        public bool IsAuthor
        {
            get;
            private set;
        }

        [NonSerialized]
        private XElement _Verb;
        public XElement Verb
        {
            get { return (_Verb); }
            private set { _Verb = value; }
        }

        public override string ToString()
        {
            return IsAuthor
                   ? "AUTHOR"
                   : (base.ToString() + ": [" + (!VerbValue.IsEmptyOrNull() ? VerbValue : "NULL") + ']');
        }
        public override int GetHashCode()
        {
            return IsAuthor
                   ? ToString().GetHashCode()
                   : base.GetHashCode();
        }
        public override bool Equals(object obj)
        {
            var e = obj as SubjectEssence;
            if ( e != null )
            {
                return IsAuthor
                       ? object.ReferenceEquals( this, e )
                       : ((this.VerbValue == e.VerbValue) && base.Equals( e ));
            }
            return base.Equals(obj);
        }

        public ObjectEssence ToObjectEssence()
        {
            if ( IsAuthor )
                throw (new InvalidOperationException("Not allowed for Author-SubjectEssence"));

            return (new ObjectEssence( AnaphoriaAndEntityTuple, true ));
        }
    }

    /// <summary>
    /// 
    /// </summary>
    [Flags]
    public enum FilterBySynonyms
    {
        UNDEFINE, 

        Subject,          //пара отфильтрована по значению субъекта
        Object,           //пара отфильтрована по значению объекта
        SubjectAndObject, //пара отфильтрована по значению субъекта и объекта

    }

    /// <summary>
    /// 
    /// </summary>
    public class ObjectEssenceEqualityComparer : IEqualityComparer< ObjectEssence >
    {
        public static ObjectEssenceEqualityComparer Instance
        {
            get;
            private set;
        }
        static ObjectEssenceEqualityComparer()
        {
            Instance = new ObjectEssenceEqualityComparer();
        }
        private ObjectEssenceEqualityComparer()
        {
        }

        #region [.IEqualityComparer< ObjectEssence >.]
        public bool Equals( ObjectEssence x, ObjectEssence y )
        {
            return (x.Equals( y ) && ObjectEntityEqualityComparer.Instance.Equals( x.AnaphoriaAndEntityTuple.Entity, y.AnaphoriaAndEntityTuple.Entity ));
        }

        public int GetHashCode(ObjectEssence obj)
        {
            return (obj.GetHashCode());
        }
        #endregion    }
    }
    /// <summary>
    /// 
    /// </summary>
    internal class ObjectEntityEqualityComparer  : IEqualityComparer< XElement >
    {
        public static ObjectEntityEqualityComparer Instance
        {
            get;
            private set;
        }
        static ObjectEntityEqualityComparer()
        {
            Instance = new ObjectEntityEqualityComparer();
        }
        private ObjectEntityEqualityComparer()
        {
        }

        #region [.IEqualityComparer< XElement >.]
        public bool Equals( XElement x, XElement y )
        {
            x.ThrowIfNull("x");
            y.ThrowIfNull("y");

            if ( x.Name                == y.Name                &&
                 x.GetAttributeVALUE() == y.GetAttributeVALUE() && 
                 x.GetAttributeID()    == y.GetAttributeID()    &&
                 x.GetAttributePOS()   == y.GetAttributePOS()   &&
                 x.GetAttributeMI()    == y.GetAttributeMI()    &&
                 x.GetAttributeROLE()  == y.GetAttributeROLE() )
            {
                var tx = x.GetAttributeTYPE();
                var ty = y.GetAttributeTYPE();
                if ( tx == ty ) 
                    return (true);
                if ( (tx == TypeAttributeValue.Inquiry) && (x.GetAttributeTYPESAVED() == ty) )
                    return (true);
            }
            return (false);
        }

        public int GetHashCode(XElement obj)
        {
            return (obj.GetHashCode());
        }
        #endregion
    }

    /// <summary>
    /// 
    /// </summary>
    [DataContract]
    [Serializable]
    public class OpinionMiningTuple
    {
        public OpinionMiningTuple( SubjectEssence subject, XElement sentence
            , int sentenceNumber, FilterBySynonyms filterBySynonyms, ICoreferenceInfo coreferenceInfo )
        {
            Initialize( subject, sentence, sentenceNumber, filterBySynonyms, coreferenceInfo );
        }
        public OpinionMiningTuple( SubjectEssence subject, ObjectEssence @object, XElement sentence
            , int sentenceNumber, FilterBySynonyms filterBySynonyms, ICoreferenceInfo coreferenceInfo )
        {
            var tempSentence = Initialize( subject, sentence, sentenceNumber, filterBySynonyms, coreferenceInfo );

            @object.ThrowIfNull("object");

            #region [.coreference-info.]
            var tuple = coreferenceInfo.TryGetMaster( @object.Entity );
            #endregion

            #region [.Find Object-entity & mark him as-inquiry.]
            var objectEntityInCopy = tempSentence.DescendantsAndSelf().FirstOrDefault( d => ObjectEntityEqualityComparer.Instance.Equals( d, @object.AnaphoriaAndEntityTuple.Entity ) );
            if ( objectEntityInCopy == null )
            {
                throw (new ArgumentException("Object-Entity not found in his Sentence. " + ("object.Entity: " + @object.AnaphoriaAndEntityTuple.Entity.ToString().InSingleQuote() + ", sentence: " + sentence.ToString().InSingleQuote()).InBrackets()));
            }
            objectEntityInCopy.MarkAsInquiry();

            var objectAnaphoriaInCopy = default(XElement);
            if ( @object.AnaphoriaAndEntityTuple.Anaphoria != null )
            {
                objectAnaphoriaInCopy = tempSentence.DescendantsAndSelf().FirstOrDefault( d => ObjectEntityEqualityComparer.Instance.Equals( d, @object.AnaphoriaAndEntityTuple.Anaphoria ) );
                if ( objectAnaphoriaInCopy == null )
                {
                    throw (new ArgumentException("Object-Anaphoria-Entity not found in his Sentence. " + ("object.Entity: " + @object.AnaphoriaAndEntityTuple.Anaphoria.ToString().InSingleQuote() + ", sentence: " + sentence.ToString().InSingleQuote()).InBrackets()));
                }
            }

            @object = new ObjectEssence( new SubjectAndAnaphoriaEntityTuple( objectEntityInCopy, objectAnaphoriaInCopy ), @object.IsSubjectIndeed );
            SentenceText = tempSentence.ToString();
            tempSentence = null;
            #endregion

            Object = @object;
            #region [.coreference-info.]
            Object.TrySetValueWithCoreferenceInfo( tuple );
            #endregion
        }
        public OpinionMiningTuple( OpinionMiningTuple other4Copy, string sentenceXml )
        {
            other4Copy.ThrowIfNull("other4Copy");
            sentenceXml.ThrowIfEmptyOrNull("sentenceXml");

            Subject          = other4Copy.Subject;
            Object           = other4Copy.Object;
            FilterBySynonyms = other4Copy.FilterBySynonyms;
            SentenceNumber   = other4Copy.SentenceNumber;
            SentenceText     = sentenceXml;  
        }

        private XElement Initialize( SubjectEssence subject, XElement sentence
            , int sentenceNumber, FilterBySynonyms filterBySynonyms, ICoreferenceInfo coreferenceInfo )
        {
            subject .ThrowIfNull("subject");
            sentence.ThrowIfNull("sentence");

            Subject          = subject;
            Subject.TrySetValueWithCoreferenceInfo( coreferenceInfo );
            SentenceNumber   = sentenceNumber;
            FilterBySynonyms = filterBySynonyms;
            var tempSentence = new XElement( sentence );

            #region [.Remove all ISSUBJECT-attribute from other than Subject.Entity elements.]
            if ( !Subject.IsAuthor )
            {
                tempSentence.RemoveOtherSubjectAttributes( Subject.AnaphoriaAndEntityTuple.Entity, ObjectEntityEqualityComparer.Instance.Equals );  
            }
            #endregion

            _Sentence    = tempSentence;
            SentenceText = tempSentence.ToString();
            return (tempSentence);
        }

        [DataMember]
        public SubjectEssence Subject
        {
            get;
            private set;
        }

        [DataMember]
        public ObjectEssence Object
        {
            get;
            private set;
        }

        [NonSerialized]
        private XElement _Sentence;
        public XElement GetSentence()
        {
            //will be work only on Wcf-service side!!!
            if ( _Sentence != null )
            {
                return (_Sentence);
            }

            return (XElement.Parse( SentenceText ));
        }

        [DataMember]
        public string SentenceText
        {
            get;
            private set;
        }

        [DataMember]
        public int SentenceNumber
        {
            get;
            private set;
        }

        [DataMember]
        public FilterBySynonyms FilterBySynonyms
        {
            get;
            private set;
        }

        public bool HasObject
        {
            get { return (Object != null); }
        }
        public void SetSentenceText( string sentenceText )
        {
            sentenceText.ThrowIfEmptyOrNull("SentenceText");

            SentenceText = sentenceText;
            _Sentence    = null;
        }

        public override string ToString()
        {
            return ( ((Subject != null) ? Subject.ToString() : "NULL") + 
                     " - " + 
                     ((Object != null) ? Object.ToString() : "NULL") );
        }

        public static OpinionMiningTuple Create4AuthorSubject( XElement @object, XElement sentence
            , int sentenceNumber, FilterBySynonyms filterBySynonyms, ICoreferenceInfo coreferenceInfo )
        {
            return (Create4AuthorSubject( new ObjectEssence( @object ), sentence
                                , sentenceNumber, filterBySynonyms, coreferenceInfo ));    
        }
        public static OpinionMiningTuple Create4AuthorSubject( ObjectEssence @object, XElement sentence
            , int sentenceNumber, FilterBySynonyms filterBySynonyms, ICoreferenceInfo coreferenceInfo )
        {
            return (new OpinionMiningTuple( SubjectEssence.Author, @object, sentence
                                , sentenceNumber, filterBySynonyms, coreferenceInfo ));
        }
    }
}
