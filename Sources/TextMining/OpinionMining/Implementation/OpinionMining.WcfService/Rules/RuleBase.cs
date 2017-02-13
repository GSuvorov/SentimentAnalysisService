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
using System.Xml.XPath;

using TextMining.Core;
using TextMining.Service;

namespace OpinionMining
{
    internal class SubjectData
    {
        public SubjectData( List< SubjectEssence > subjectEssences, XElement leftExtremeElement, XElement rightExtremeElement )
        {
            subjectEssences.ThrowIfNull("subjectEssences");
            if ( !subjectEssences.Any() ) throw (new ArgumentNullException("subjectEssences"));
            leftExtremeElement .ThrowIfNull("leftExtremeElement");
            rightExtremeElement.ThrowIfNull("rightExtremeElement");

            SubjectEssences     = subjectEssences.AsReadOnly();
            LeftExtremeElement  = leftExtremeElement;
            RightExtremeElement = rightExtremeElement;
        }

        public ReadOnlyCollection< SubjectEssence > SubjectEssences
        {
            get;
            private set;
        }

        public XElement LeftExtremeElement
        {
            get;
            private set;
        }
        public XElement RightExtremeElement
        {
            get;
            private set;
        }

        public override string ToString()
        {
            return (string.Join(", ", SubjectEssences.Select(_ => _.ToString()).ToArray()));
        }
        public override int GetHashCode()
        {
            var hash = 0;
            foreach ( var _ in SubjectEssences )
            {
                hash ^= _.GetHashCode();
            }
            return (hash);

            /*unchecked
            {
                return SubjectEssences.Sum( _ => _.GetHashCode() );
            }*/
        }
        public override bool Equals(object obj)
        {
            var e = obj as SubjectData;
            if ( e != null )
            {
                return (SubjectEssences.SequenceEqual( e.SubjectEssences ));
            }
            return base.Equals(obj);
        }
    }
    //===================================================//

    internal class SubjectObjectsTuple
    {
        public SubjectObjectsTuple( IEnumerable< SubjectData > subjects, IEnumerable< XElement > objects, string ruleId )
        {
            subjects.ThrowIfNull("subjects");
            objects .ThrowIfNull("objects");
            ruleId  .ThrowIfNull("ruleId");

            Subjects = subjects.ToList().AsReadOnly(); 
            Objects  = (from o in objects
                        select new ObjectEssence( o )
                       )
                       .ToList()
                       .AsReadOnly();
            RuleId = ruleId;
        }
        public SubjectObjectsTuple( IEnumerable< SubjectData > subjects, IEnumerable< ObjectEssence > objects, string ruleId )
        {
            subjects.ThrowIfNull("subjects");
            objects .ThrowIfNull("objects");
            ruleId  .ThrowIfNull("ruleId");

            Subjects = subjects.ToList().AsReadOnly();  
            Objects  = objects .ToList().AsReadOnly();
            RuleId   = ruleId;
        }

        public ReadOnlyCollection< SubjectData > Subjects
        {
            get;
            private set;
        }
        public ReadOnlyCollection< ObjectEssence > Objects
        {
            get;
            private set;
        }
        public string RuleId
        {
            get;
            private set;
        }        
    }
    //===================================================//

    internal abstract class RuleBase
    {
        public RuleBase( string id = null )
        {
            Id = id.IsEmptyOrNull() ? this.GetType().Name : id;
        }

        public string Id
        {
            get;
            private set;
        }
        public override string ToString()
        {
            return ( ' ' + ("rule " + Id.InSingleQuote()).InBrackets() );
        }

        #region [.DEBUG.]
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
        /*public void MatchActionDebugInfoOutput( actionBase action )
        {
#if DEBUG
            //Console.WriteLine( "\t    Match action #" + action.Position + " in rule: " + this.Id.InSingleQuote() );

            AddDebugInfo2Dictionary( "\r\n\t    Match action #" + action.Position + " in rule: " + this.Id.InSingleQuote() );

            AddOperateRuleName2Dictionary();
#endif
        }*/
        public static void MatchActionDebugInfoOutput( string ruleId )
        {
#if DEBUG
            //Console.WriteLine( "\t    Match action #1 in rule: " + ruleId.InSingleQuote() );

            AddDebugInfo2Dictionary( "\r\n\t    Match action #1 in rule: " + ruleId.InSingleQuote() );

            AddOperateRuleName2Dictionary( ruleId );
#endif
        }
        public void NomatchActionDebugInfoOutput()
        {
#if DEBUG
            //Console.WriteLine("\t   No match actions in rule: " + this.Id.InSingleQuote());

            AddDebugInfo2Dictionary( "\r\n\t   No match actions in rule: " + this.Id.InSingleQuote() );
#endif
        }
        public static void Message2Console( string message )
        {
#if DEBUG
            Console.WriteLine( message );
#endif
        }
        //=====================================================================//
        #endregion
    }
    //===================================================//

    internal abstract class GroupBase< TRule, TEssence > where TRule : RuleBase
    {
        protected TRule[] _Rules;
        protected GroupBase( TRule[] rules )
        {
            rules.ThrowIfNull("rules");

            _Rules = rules;
        }

        protected static IEnumerable< SubjectObjectsTuple > GroupAlikeSubjects( IEnumerable< SubjectObjectsTuple > opinionMiningTuples )
        {
            var result = from t in opinionMiningTuples
                         group t.Objects by new { Subjects = t.Subjects, RuleId = t.RuleId } into group_by_subjects
                         select new SubjectObjectsTuple
                         (
                             group_by_subjects.Key.Subjects,
                             group_by_subjects.SelectMany( _ => _ ).Distinct( ObjectEssenceEqualityComparer.Instance ),
                             group_by_subjects.Key.RuleId
                         );

            return (result);

            #region commented
            /*
            var x = from t in opinionMiningTuples
                    group t.Objects by t.Subjects into group_by_subjects
                    select new SubjectObjectsTuple
                    (
                        group_by_subjects.Key, 
                        group_by_subjects.SelectMany( _ => _ ) 

                    );

            return (x);             
            */
            #endregion
        }

        protected abstract TEssence[] GetEssence4Processing( XElement sent );
        protected abstract SubjectObjectsTuple ProcessEssence( TRule rule, TEssence essence, Language language
            , ref int directAndIndirectSpeechGlobalNumber, ObjectAllocateMethod objectAllocateMethod );
        public abstract IEnumerable< SubjectObjectsTuple > Process( XElement sent, Language languag
            , ref int directAndIndirectSpeechGlobalNumber, ObjectAllocateMethod objectAllocateMethod );
    }
    //===================================================//

    internal abstract class DS_GroupBase< TRule, TEssence > : GroupBase< TRule, TEssence > where TRule : RuleBase
    {
        protected DS_GroupBase( TRule[] rules ) : base( rules )
        {
        }

        public override IEnumerable< SubjectObjectsTuple > Process( XElement sent, Language language
            , ref int directAndIndirectSpeechGlobalNumber, ObjectAllocateMethod objectAllocateMethod )
        {
            sent.ThrowIfNotSent();

            var opinionMiningTuples = new List< SubjectObjectsTuple >();
            var essences = GetEssence4Processing( sent );
            foreach ( var essence in essences )
            {
                foreach ( var rule in _Rules )
                {
                    var opinionMiningTuple = ProcessEssence( rule, essence, language, ref directAndIndirectSpeechGlobalNumber, objectAllocateMethod );
                    if ( !opinionMiningTuple.IsNull() )
                    {
                        opinionMiningTuples.Add( opinionMiningTuple );
                        break;
                    }
                }
            }

            //Объединяем/группируем одинаковые Subject/субъекты
            return (GroupAlikeSubjects( opinionMiningTuples ));
        }
    }

    internal abstract class IS_GroupBase< TRule, TEssence > : GroupBase< TRule, TEssence > where TRule : RuleBase
    {
        protected IS_GroupBase( TRule[] rules ) : base( rules )
        {
        }

        public override IEnumerable< SubjectObjectsTuple > Process( XElement sent, Language language
            , ref int directAndIndirectSpeechGlobalNumber, ObjectAllocateMethod objectAllocateMethod )
        {
            sent.ThrowIfNotSent();

            var opinionMiningTuples = new List< SubjectObjectsTuple >();            
            var opinionMiningTuple  = default(SubjectObjectsTuple);
            var essences = GetEssence4Processing( sent );
            foreach ( var essence in essences )
            {
                foreach ( var rule in _Rules )
                {
                    opinionMiningTuple = ProcessEssence( rule, essence, language, ref directAndIndirectSpeechGlobalNumber, objectAllocateMethod );
                    if ( !opinionMiningTuple.IsNull() )
                    {
                        opinionMiningTuples.Add( opinionMiningTuple );
                        break;
                    }
                }

                if ( !opinionMiningTuple.IsNull() )
                {
                    break;
                }
            }

            //Объединяем/группируем одинаковые Subject/субъекты
            return (GroupAlikeSubjects( opinionMiningTuples ));
        }
    }
    //===================================================//
}
