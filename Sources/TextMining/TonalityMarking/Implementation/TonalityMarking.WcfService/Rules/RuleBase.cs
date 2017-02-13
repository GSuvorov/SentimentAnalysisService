using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.IO;
using System.Linq;
using System.ServiceModel;
using System.Text;
using System.Threading;
using System.Xml;
using System.Xml.Linq;

using TextMining.Core;
using TextMining.Service;

namespace TonalityMarking
{
    internal enum RuleCycleType
    {
        None,
        RecycleOnSuccsess,
    }

    internal abstract class actionBase
    {
        public actionBase( XElement actionElement )
        {
            actionElement.ThrowIfNull("actionElement");

            Position = actionElement.GetPositionInParent();

            FRT_toAdd = actionElement.GetAttributeFRT();
        }

        public int Position
        {
            get;
            private set;
        }

        public int FRT_toAdd
        {
            get;
            private set;
        }
    }

    internal abstract class RuleBase
    {
        public RuleBase( XElement ruleNode )
        {
            ruleNode.ThrowIfNull("ruleNode");

            var id = ruleNode.GetAttribute( "id" );

            id.ThrowIfEmptyOrNull("id" );

            Id = id;
        }

        public string Id
        {
            get;
            private set;
        }

        public override string ToString()
        {
            return (Id);
        }

        public abstract bool Process( XElement node );
        public abstract RuleCycleType RuleCycle { get; }


        //=====================================================================//
#if DEBUG
        private static readonly Dictionary< string, StringBuilder > _DebugInfoDictionary = new Dictionary< string, StringBuilder >();
        private static readonly Dictionary< string, IList< string > > _OperateRulesDebugInfoDictionary = new Dictionary< string, IList< string > >();
        private static string ContextKey
        {
            get { return (OperationContext.Current.SessionId ?? Thread.CurrentThread.ManagedThreadId.ToString()); }
        }
#endif
        public static string GetDebugInfoOutput( bool clearAfterGet = true )
        {
#if DEBUG
            if ( WcfHelper.IsAllowedWCF )
            {
                lock ( _DebugInfoDictionary )
                {
                    var sb = default(StringBuilder);
                    if ( _DebugInfoDictionary.TryGetValue( ContextKey, out sb ) )
                    {
                        if ( clearAfterGet ) 
                            _DebugInfoDictionary.Remove( ContextKey );

                        return (sb.ToString());
                    }
                }
            }
#endif
            return (null);
        }
        public static void ClearDebugInfoOutput()
        {
#if DEBUG
            if ( WcfHelper.IsAllowedWCF )
            {
                lock ( _DebugInfoDictionary )
                {
                    _DebugInfoDictionary.Remove( ContextKey );
                }
            }
#endif
        }
        public static ReadOnlyCollection< string > GetOperateRulesNamesDebugInfoOutput( bool clearAfterGet = true )
        {
#if DEBUG
            if ( WcfHelper.IsAllowedWCF )
            {
                lock ( _DebugInfoDictionary )
                {
                    var listOfRules = default(IList< string >);
                    if ( _OperateRulesDebugInfoDictionary.TryGetValue( ContextKey, out listOfRules ) )
                    {
                        if ( clearAfterGet ) 
                            _OperateRulesDebugInfoDictionary.Remove( ContextKey );

                        return (listOfRules.ToList().AsReadOnly());
                    }
                }
            }
#endif
            return (null);
        }
        public static void ClearOperateRulesDebugInfoOutput()
        {
#if DEBUG
            if ( WcfHelper.IsAllowedWCF )
            {
                lock ( _DebugInfoDictionary )
                {
                    _OperateRulesDebugInfoDictionary.Remove( ContextKey );
                }
            }
#endif
        }
        private static void AddDebugInfo2Dictionary( string value )
        {
#if DEBUG
            if ( WcfHelper.IsAllowedWCF )
            {
                lock ( _DebugInfoDictionary )
                {
                    var sb = default(StringBuilder);
                    if ( !_DebugInfoDictionary.TryGetValue( ContextKey, out sb ) )
                    {
                        sb = new StringBuilder();
                        _DebugInfoDictionary.Add( ContextKey, sb );
                    }

                    sb.Append( value );
                }
            }
#endif
        }
        private static void AddOperateRuleName2Dictionary( string ruleId )
        {
#if DEBUG
            ruleId.ThrowIfEmptyOrNull("ruleId");

            if ( WcfHelper.IsAllowedWCF )
            {
                lock ( _DebugInfoDictionary )
                {
                    var listOfRules = default(IList< string >);
                    if ( !_OperateRulesDebugInfoDictionary.TryGetValue( ContextKey, out listOfRules ) )
                    {
                        listOfRules = new List< string >();
                        _OperateRulesDebugInfoDictionary.Add( ContextKey, listOfRules );
                    }

                    listOfRules.Add( ruleId );
                }
            }
#endif
        }
        private void AddOperateRuleName2Dictionary()
        {
#if DEBUG
            AddOperateRuleName2Dictionary( this.Id );
#endif
        }

        public void MatchRuleDebugInfoOutput( XElement element )
        {
#if DEBUG
            //Console.WriteLine( '\t' + (element.Name.LocalName + " #" + element.GetPositionInParent()).InBrackets() + " => Match rule: " + this.Id.InSingleQuote() );
            
            AddDebugInfo2Dictionary( "\r\n\t" + (element.Name.LocalName + " #" + element.GetPositionInParent()).InSingleQuote() + " => Match rule: " + this.Id.InSingleQuote() );
#endif
        }
        public void MatchActionDebugInfoOutput( actionBase action )
        {
#if DEBUG
            AddDebugInfo2Dictionary( "\r\n\t    +Match action #" + action.Position + " in rule: " + this.Id.InSingleQuote() );

            AddOperateRuleName2Dictionary();
#endif
        }
        public void NomatchActionDebugInfoOutput()
        {
#if DEBUG
            AddDebugInfo2Dictionary( "\r\n\t    ~No match actions in rule: " + this.Id.InSingleQuote() );
#endif
        }

        public static void MatchActionDebugInfoOutput( string ruleId )
        {
#if DEBUG
            AddDebugInfo2Dictionary("\r\n\t    +Match action #1 in rule: " + ruleId.InSingleQuote());

            AddOperateRuleName2Dictionary( ruleId );
#endif
        }
        public static void NomatchActionDebugInfoOutput( string ruleId )
        {
#if DEBUG
            AddDebugInfo2Dictionary("\r\n\t    ~No match actions in rule: " + ruleId.InSingleQuote());
#endif
        }
        //=====================================================================//
    }
}
