using System;
using System.Collections.Generic;
using System.Linq;
using System.Xml.Linq;
using System.Xml.XPath;

using TextMining.Core;

namespace TonalityMarking
{
    /// <summary>
    /// 
    /// </summary>
    internal enum TypeOfXmlElement
    {
        Undefined,

        U,
        ENTITY,
        VERB,


        ENTITIES,

        CET,
    }

    /// <summary>
    /// 
    /// </summary>
    internal class ProcessingItem
    {
        private bool _AllowUnknownNodeType;

        public ProcessingItem( XElement element )
            : this( element, false )
        {            
        }
        public ProcessingItem( XElement element, bool allowUnknownNodeType )
        {
            if ( element.IsElementU() )
            {
                this.TypeOfXmlElement = TypeOfXmlElement.U;
            }
            else if ( element.IsElementENTITY() )
            {
                this.TypeOfXmlElement = TypeOfXmlElement.ENTITY;
            }
            else if ( element.IsElementVERB() )
            {
                this.TypeOfXmlElement = TypeOfXmlElement.VERB;
            }
            else if ( element.IsElementENTITIES() )
            {
                this.TypeOfXmlElement = TypeOfXmlElement.ENTITIES;
            }
            else if ( element.IsElementCET() )
            {
                this.TypeOfXmlElement = TypeOfXmlElement.CET;
            }                
            else
            {
                if ( allowUnknownNodeType )
                    this.TypeOfXmlElement = TypeOfXmlElement.Undefined;
                else
                    throw (new InvalidOperationException("Operation allowed only for 'U', 'ENTITY', 'VERB', 'ENTITIES', 'CET' elements."));
            }

            XElement = element;
            _AllowUnknownNodeType = allowUnknownNodeType;
        }

        public XElement XElement
        {
            get;
            private set;
        }
        public TypeOfXmlElement TypeOfXmlElement
        {
            get;
            private set;
        }

        public ProcessingItem NextSibling
        {
            get
            {
                var nextSibling = XElement.ElementsAfterSelfExceptT().FirstOrDefault();
                if ( nextSibling == null )
                    return (null);

                if ( nextSibling.IsElementSUBSENT() )//|| nextSibling.IsElementSKIPSUBSENT() )
                    return (null);

                if ( nextSibling.IsElementSENT() && nextSibling.GetAttributeSUBSENTTYPE() == SubsentenceType.DirectSpeech )
                    return (null);

                return (new ProcessingItem( nextSibling, _AllowUnknownNodeType ));
            }
        }

        public RoleAttributeValue? Role
        {
            get { return (XElement.GetAttributeROLE()); }
        }
        public TypeAttributeValue? TYPE
        {
            get { return (XElement.GetAttributeTYPE()); }
        }

        private string _PRE;
        public virtual string PRE
        {
            get 
            { 
                if ( _PRE == null )
                {
                    _PRE = XElement.GetAttributePRE().ToLowerEx()
                                   .ReplaceSingleQuote();
                }
                return (_PRE); 
            }
        }

        private string _PROF;
        public virtual string PROF
        {
            get 
            {
                if ( _PROF == null )
                {
                    _PROF = XElement.GetAttributePROF().ToLowerEx()
                                    .ReplaceSingleQuote();
                }
                return (_PROF);
            }
        }

        private string _VALUE;
        public virtual string VALUE
        {
            get 
            {
                if ( _VALUE == null )
                {
                    _VALUE = XElement.GetAttributeVALUE().ToLowerEx()
                                     .ReplaceSingleQuote();
                }
                return (_VALUE);
            }
        }

        private string _CDataText4ElementU;
        public virtual string CDataText4ElementU
        {
            get
            {
                if ( this.TypeOfXmlElement == TypeOfXmlElement.U )
                {
                    if ( _CDataText4ElementU == null )
                    {
                        _CDataText4ElementU = XElement.GetCDataTextFromElementU().ToLowerEx();
                    }
                    return (_CDataText4ElementU);
                }
                throw (new InvalidCastException("Allowed only for 'U' elements"));
            }
        }

        public override string ToString()
        {
            return (TypeOfXmlElement.ToString() + '(' + VALUE + ')');
        }
    }

    /// <summary>
    /// 
    /// </summary>
    internal class CollocationProcessingItem : ProcessingItem
    {
        public CollocationProcessingItem( XElement element )
            : base( element )
        {
        }

        private List< string > _ChainOfWords;
        private List< string > ChainOfWords
        {
            get
            {
                if ( _ChainOfWords == null )
                {
                    _ChainOfWords = this.GetChainOfWords();
                }
                return (_ChainOfWords);
            }
        }
        private List< string > GetChainOfWords()
        {
            switch ( this.TypeOfXmlElement )
            {
                case TypeOfXmlElement.U:
                    return (this.CDataText4ElementU.SplitBySpace().ToList());

                case TypeOfXmlElement.ENTITY:
                case TypeOfXmlElement.VERB:
                    var list = new List< string >();
                    
                    if ( !base.PROF.IsEmptyOrNull() )
                    {
                        if ( !base.PRE.IsEmptyOrNull() )
                        {
                            foreach ( var pre in base.PRE.SplitBySpace() )
                            {
                                list.Add( pre.ReplaceEnd2Vowels() );
                            }
                        }
                        list.AddRange( base.PROF.SplitBySpace() );
                    }
                    else 
                    {
                        list.AddRange( base.VALUE.SplitBySpace() );
                    }
                    
                    #region [.commented. previous.]
                    /*if ( !base.PRE.IsEmptyOrNull() && !base.PROF.IsEmptyOrNull() )
                    {
                        foreach ( var pre in base.PRE.SplitBySpace() )
                        {
                            list.Add( pre.ReplaceEnd2Vowels() );
                        }

                        list.AddRange( base.PROF.SplitBySpace() );
                    }
                    else
                    {
                        list.AddRange( base.VALUE.SplitBySpace() );
                    }*/
                    #endregion

                return (list);

                case TypeOfXmlElement.CET:
                    return (base.VALUE.SplitBySpace().ToList());

                default:
                    throw (new ArgumentException( this.TypeOfXmlElement.ToString() ));
            }            
        }

        private CollocationProcessingItem NextCollocationSibling
        {
            get
            {
                XElement nextSibling;
                if ( XElement.HasElementsExceptT() )
                {
                    nextSibling = XElement.ElementsExceptT().FirstOrDefault();

                    if ( nextSibling.IsNull() || nextSibling.IsElementSUBSENT() )
                        return (null);
                }
                else
                {
                    nextSibling = XElement.ElementsAfterSelfExceptT().FirstOrDefault();
                    if ( nextSibling == null )
                    {
                        var parent = XElement.Parent;

                        while ( (parent != null) &&
                                 parent.IsElementCET()      || parent.IsElementENTITY() || 
                                 parent.IsElementENTITIES() || parent.IsElementVERB() )
                        {
                            nextSibling = parent.ElementsAfterSelfExceptT().FirstOrDefault();
                            if ( nextSibling != null )
                                break;

                            parent = parent.Parent;
                        }
                    }

                    if ( nextSibling.IsNull() || nextSibling.IsElementSUBSENT() )
                        return (null);

                    if ( nextSibling.IsElementENTITY() && nextSibling.IsTypeInquiry() )
                        return (null);

                    if ( nextSibling.IsElementSENT() && nextSibling.GetAttributeSUBSENTTYPE() == SubsentenceType.DirectSpeech )
                        return (null);
                }

                return (new CollocationProcessingItem( nextSibling ));
            }
        }

        public string FirstWordInChainOfWords
        {
            get { return ((ChainOfWords.Count != 0) ? ChainOfWords[ 0 ] : null); }
        }
        public Pair< IList< string >, IList< CollocationProcessingItem > > GetChainOfWords4Compare( int countOfWordInChain )
        {
            var words = new List< string >();
            var items = new List< CollocationProcessingItem >();

            items.Add( this );

            for ( int i = 1; i < Math.Min( countOfWordInChain + 1, this.ChainOfWords.Count ); i++ )
            {
                words.Add( this.ChainOfWords[ i ] );
            }

            if ( countOfWordInChain <= this.ChainOfWords.Count - 1 )
            {
                return (new Pair< IList< string >, IList< CollocationProcessingItem > >( words, items ));
            }
            else
            {
                countOfWordInChain -= this.ChainOfWords.Count - 1;
                for ( var ns  = this.NextCollocationSibling; 
                          ns != null; 
                          ns  = ns.NextCollocationSibling 
                          )
                {
                    for ( int i = 0; i < Math.Min( countOfWordInChain, ns.ChainOfWords.Count ); i++ )
                    {
                        words.Add( ns.ChainOfWords[ i ] );
                    }
                    items.Add( ns );

                    countOfWordInChain -= ns.ChainOfWords.Count;

                    if ( countOfWordInChain <= 0 )
                    {
                        return (new Pair< IList< string >, IList< CollocationProcessingItem > >( words, items ));
                    }
                }
            }

            return (null);
        }
    }

    /// <summary>
    /// 
    /// </summary>
    internal class LexemeVerbProcessingItem : ProcessingItem
    {
        public LexemeVerbProcessingItem( XElement element )
            : base( element )
        {
            if ( !this.XElement.IsElementVERB() )
                throw (new ArgumentException("!this.XElement.IsElementVERB()"));
        }
        private LexemeVerbProcessingItem( XElement element, bool allowUnknownNodeType )
            : base( element, allowUnknownNodeType )
        {
        }

        private LexemeVerbProcessingItem NextLexemeVerbSibling
        {
            get
            {
                XElement nextSibling;
                if ( XElement.HasElementsExceptT() )
                {
                    nextSibling = XElement.ElementsExceptT().FirstOrDefault();
                }
                else
                {
                    nextSibling = XElement.ElementsAfterSelfExceptT().FirstOrDefault();
                    if ( nextSibling == null )
                    {
                        var parent = XElement.Parent;

                        while ( (parent != null) &&
                                 parent.IsElementCET()      || parent.IsElementENTITY() || 
                                 parent.IsElementENTITIES() || parent.IsElementVERB() )
                        {
                            nextSibling = parent.ElementsAfterSelfExceptT().FirstOrDefault();
                            if ( nextSibling != null )
                                break;

                            parent = parent.Parent;
                        }
                    }
                }
                if ( nextSibling == null )
                    return (null);

                if ( nextSibling.IsElementSUBSENT() )//|| nextSibling.IsElementSKIPSUBSENT() )
                    return (null);

                if ( nextSibling.IsElementSENT() && nextSibling.GetAttributeSUBSENTTYPE() == SubsentenceType.DirectSpeech )
                    return (null);

                //только для Collocation!!!
                /*if ( nextSibling.IsElementENTITY() && nextSibling.IsTypeInquiry() )
                    return (null);*/

                return (new LexemeVerbProcessingItem( nextSibling, true ));
            }
        }

        public IList< string > GetWordsTillSentenceEnd()
        {
            var list = new List< string >();

            for ( var ns  = this.NextLexemeVerbSibling; 
                      ns != null; 
                      ns  = ns.NextLexemeVerbSibling 
                      )
            {
                switch ( ns.TypeOfXmlElement )
                {
                    case TypeOfXmlElement.U:
                        if ( ns.CDataText4ElementU.ContainsSignOfPunctuation() )
                        {
                            return (list);
                        }
                        list.AddRange( ns.CDataText4ElementU.SplitBySpace() );
                    break;

                    case TypeOfXmlElement.CET:
                        if ( (ns.TYPE == TypeAttributeValue.Pretext) && !ns.VALUE.IsEmptyOrNull() )
                        {
                            list.Add( ns.VALUE );
                        }
                    break;

                    /*Alex say - it's delirium!!!
                    case TypeOfXmlElement.ENTITY:
                        if ( ns.Role == RoleAttributeValue.Adverb )
                        {
                            list.Add( ns.VALUE );
                        }
                    break;*/
                }
            }

            return (list);
        }
        /*public IList< string > GetWordsTillSentenceEnd()
        {
            var list = new List< string >();

            var nextSibling = XElement.ElementsAfterSelfExceptT().FirstOrDefault();
            if ( nextSibling != null )
            {
                for ( var ns  = new ProcessingItem( nextSibling, true ); 
                          ns != null; 
                          ns  = ns.NextSibling 
                          )
                {
                    switch ( ns.TypeOfXmlElement )
                    {
                        case TypeOfXmlElement.U:
                            if ( ns.CDataText4ElementU.ContainsSignOfPunctuation() )
                            {
                                return (list);
                            }
                            list.AddRange( ns.CDataText4ElementU.SplitBySpace() );
                        break;

                        case TypeOfXmlElement.CET:
                            if ( !ns.VALUE.IsEmptyOrNull() )
                            {
                                list.Add( ns.VALUE );
                            }
                        break;

                        case TypeOfXmlElement.ENTITY:
                            if ( ns.Role == RoleAttributeValue.Adverb )
                            {
                                list.Add( ns.VALUE );
                            }
                        break;
                    }
                }
            }

            return (list);
        }*/
    }

    /// <summary>
    /// 
    /// </summary>
    internal class LexemeEntityProcessingItem : ProcessingItem
    {
        public LexemeEntityProcessingItem( XElement element )
            : base( element )
        {    
            if ( !this.XElement.IsElementENTITY() )
                throw (new ArgumentException("!this.XElement.IsElementENTITY()"));

            switch ( this.RealTYPE )
            {
                case TypeAttributeValue.Adverb:
                    this.AdverbsWord = base.VALUE.Trim();
                break;

                case TypeAttributeValue.Adjective:
                case TypeAttributeValue.ShortAdjective: 
                    if ( base.PRE.IsEmptyOrNull() )
                    {
                        this.AdjectivesWords = new[] { base.VALUE.ReplaceEnd2Vowels() };
                    }
                    else
                    {
                        this.AdjectivesWords = new[] { base.PRE.ReplaceEnd2Vowels() };
                        this.HasPREwithOneWord = true;
                    }
                break;

                default:
                    if ( !base.PRE.IsEmptyOrNull() )
                    {
                        this.AdjectivesWords = (from pre in base.PRE.SplitBySpace()
                                                select pre.ReplaceEnd2Vowels()
                                               )
                                               .ToArray();
                        this.HasPREwithOneWord = (this.AdjectivesWords.Length == 1);
                    }

                    this.NounWord = (!base.PROF.IsEmptyOrNull()) ? base.PROF : base.VALUE;
                break;
            }
        }

        private TypeAttributeValue? _RealTYPE;
        public TypeAttributeValue? RealTYPE
        {
            get
            {
                if ( !_RealTYPE.HasValue )
                {
                    _RealTYPE = XElement.GetAttributeTYPESAVEDorTYPE();
                }
                return (_RealTYPE);
            }
        }
        public string NounWord
        {
            get;
            private set;
        }
        public string[] AdjectivesWords
        {
            get;
            private set;
        }
        public string AdverbsWord
        {
            get;
            private set;
        }

        public bool HasPREwithOneWord
        {
            get;
            private set;
        }        
        public bool HasFirstChildVerb( out Verbs? snt, out int frt )
        {            
            var verb = XElement.Elements().FirstOrDefault();
            if ( verb != null && verb.IsElementVERB() )
            {
                snt = (Verbs) verb.GetAttributeSNT();//.ToString().ToEnum< Verbs >();
                frt = verb.GetAttributeFRT();
                return (true);
            }
            
            snt = null;
            frt = 0;
            return (false);
        }
    }

    /// <summary>
    /// 
    /// </summary>
    internal class FlatChainRuleProcessingItem : ProcessingItem
    {
        public FlatChainRuleProcessingItem( XElement element )
            : base( element )
        {
        }

        public FlatChainRuleProcessingItem NextSiblingFlatChain
        {
            get
            {
                var nextSibling = XElement.ElementsAfterSelfExceptT().FirstOrDefault();
                if ( nextSibling == null )
                    return (null);

                if ( nextSibling.IsElementSUBSENT() )//|| nextSibling.IsElementSKIPSUBSENT() )
                    return (null);

                if ( nextSibling.IsElementSENT() && nextSibling.GetAttributeSUBSENTTYPE() == SubsentenceType.DirectSpeech )
                    return (null);

                return (new FlatChainRuleProcessingItem( nextSibling ));
            }
        }

        private string[] _Uvalue;
        public string[] Uvalue
        {
            get
            {
                if ( (this.TypeOfXmlElement == TypeOfXmlElement.U) && (_Uvalue == null)  )
                {
                    _Uvalue = this.CDataText4ElementU
                                  .Replace( '—', '-' )
                                  .Replace( '–', '-' ) //!заменяем длинное тире на короткое!
                                  .SplitBySpace();
                }
                return (_Uvalue);
            }
        }

        private string[] _CETvalue;
        public string[] CETvalue
        {
            get
            {
                if ( (this.TypeOfXmlElement == TypeOfXmlElement.CET) && (_CETvalue == null) )
                {
                    _CETvalue = this.VALUE
                                    .SplitBySpace();
                }
                return (_CETvalue);
            }
        }
    }
}