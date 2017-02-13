using System;
using System.Collections.Generic;
using System.Linq;
using System.Xml.Linq;
using System.Xml.XPath;

using TextMining.Core;

namespace OpinionMining
{
    internal enum TypeOfXmlElement
    {
        Undefined,

        U,
        ENTITY,
        VERB,


        ENTITIES,

        CET,
    }

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
                    _PRE = XElement.GetAttributePRE().ToLowerEx();    
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
                    _PROF = XElement.GetAttributePROF().ToLowerEx();
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
                    _VALUE = XElement.GetAttributeVALUE().ToLowerEx();
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


    internal class VerbCollocationProcessingItem : ProcessingItem
    {
        public VerbCollocationProcessingItem( XElement element )
            : base( element, true )
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
                        /*убрали @PRE - типа не нужен
                        if ( !base.PRE.IsEmptyOrNull() )
                        {
                            foreach ( var pre in base.PRE.SplitBySpace() )
                            {
                                list.Add( pre.ReplaceEnd2Vowels() );
                            }
                        }
                        */
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
                    return (null); //throw (new ArgumentException( this.TypeOfXmlElement.ToString() ));
            }            
        }

        public VerbCollocationProcessingItem NextCollocationSibling
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

                if ( nextSibling.IsElementENTITY() && nextSibling.IsTypeInquiry() )
                    return (null);

                return (new VerbCollocationProcessingItem( nextSibling ));

                #region [.commented. previous.]
                /*
                var nextSibling = XElement.ElementsAfterSelfExceptT().FirstOrDefault();
                if ( nextSibling == null )
                    return (null);

                if ( nextSibling.IsElementSUBSENT() || nextSibling.IsElementSKIPSUBSENT() )
                    return (null);

                if ( nextSibling.IsElementENTITY() && nextSibling.IsTypeInquiry() )
                    return (null);

                return (new CollocationProcessingItem( nextSibling ));
                */
                #endregion
            }
        }

        public string FirstWordInChainOfWords
        {
            get { return (ChainOfWords.AnyEx() ? ChainOfWords.First() : null); }
        }
        public Tuple< IList< string >, IList< VerbCollocationProcessingItem > > GetChainOfWords4Compare( int countOfWordInChain )
        {
            var words = new List< string >();
            var items = new List< VerbCollocationProcessingItem >();

            items.Add( this );

            for ( int i = 1; i < Math.Min( countOfWordInChain + 1, this.ChainOfWords.Count ); i++ )
            {
                words.Add( this.ChainOfWords[ i ] );
            }

            if ( countOfWordInChain <= this.ChainOfWords.Count - 1 )
            {
                return (new Tuple< IList< string >, IList< VerbCollocationProcessingItem > >( words, items ));
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
                        return (new Tuple< IList< string >, IList< VerbCollocationProcessingItem > >( words, items ));
                    }
                }
            }

            return (null);
        }

        public bool IsTypeOfXmlElement_Cet_U_Entity
        {
            get
            {
                switch ( TypeOfXmlElement )
                {
                    case TypeOfXmlElement.CET:
                    case TypeOfXmlElement.U:
                    case TypeOfXmlElement.ENTITY:
                        return (true);
                }

                return (false);
            }
        }
    }
}