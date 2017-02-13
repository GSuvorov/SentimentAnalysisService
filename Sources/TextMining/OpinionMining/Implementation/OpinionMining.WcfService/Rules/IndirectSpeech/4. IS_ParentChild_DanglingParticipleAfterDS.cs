using System;
using System.Collections.Generic;
using System.Linq;
using System.Xml;
using System.Xml.Linq;

namespace OpinionMining
{
    using OpinionMining.DirectIndirectSpeechExtensions;
    using OpinionMining.SubjectsVerbExtensions;
    using TextMining.Core;
    using SubsentTuple = IS_ParentChild_DanglingParticipleBase.SubsentTuple;

    //==================== After DirectSpeech specially ====================//   
    internal abstract class IS_ParentChild_DanglingParticipleAfterDSBase : RuleBase
    {
        public IS_ParentChild_DanglingParticipleAfterDSBase( string id = null ) : base( id )
        {
        }

        public static IEnumerable< IS_ParentChild_DanglingParticipleBase.SubsentTuple > GetSubsentTuples( XElement sent )
        {
            sent.ThrowIfNotSent();

            var tuples = from parent in sent.DescendantsSubsentence()
                         where parent.AttributeTypeAreEqual( SubsentenceType.Subordinate,
                                                             SubsentenceType.Default, 
                                                             SubsentenceType.Introductory )

                         from child_dp in parent.ElementsSubsentence()
                         where child_dp.AttributeTypeAreEqual( SubsentenceType.DanglingParticiple )

                         from child in child_dp.ElementsSubsentence()
                         where child.AttributeTypeAreEqual( SubsentenceType.Subordinate,
                                                            SubsentenceType.Default, 
                                                            SubsentenceType.Introductory )

                         /*where ( parent  .IsSubsentNotHasSubject() && 
                                 child_dp.IsSubsentNotHasSubject() && 
                                 child   .IsSubsentNotHasSubject() )*/

                         select new SubsentTuple( parent, child_dp, child );

            return (tuples);
        }

        protected abstract bool IsParentMatchCondition( SubsentTuple tuple, Language language, out IEnumerable< SubjectData > subjectData );
        protected abstract bool IsChildMatchCondition( SubsentTuple tuple, Language language );

        public virtual SubjectObjectsTuple Process( SubsentTuple tuple, Language language, int directAndIndirectSpeechGlobalNumber
                                                    , ObjectAllocateMethod objectAllocateMethod, SubjectObjectsTuple DSsubjectObjectsTuple )
        {
        #region [.condition.]
            var subjects = default(IEnumerable< SubjectData >);
            if ( !IsParentMatchCondition( tuple, language, out subjects ) || 
                 !IsChildMatchCondition( tuple, language ) )
            {
                return (null); 
            }
        #endregion

            //проверяем, что субъект высказывания Entity из DS остался тот же 
            var e1 = from s1 in DSsubjectObjectsTuple.Subjects
                     from s2 in s1.SubjectEssences
                     select s2.Entity;
            var e2 = from s1 in subjects
                     from s2 in s1.SubjectEssences
                     select s2.Entity;
            if ( e2.Except( e1 ).Any() )
            {
                return (null);
            }

        #region [.match condition.]
            MatchActionDebugInfoOutput( this.Id );

            //set 'ISSUBJECT' attribute for subject-entity
            subjects.SetAttributeISSUBJECTforAllEntity( directAndIndirectSpeechGlobalNumber );

            //set 'ISOBJECT' attribute for all object-entity
            var id = subjects.GetAttributeIDForce();            

            //allocate objects
            var subsents = tuple.CHILD.AfterAndSelfSubsents().ToArray();
            var objects  = subsents.TryAllocateObjectsInSubsents( objectAllocateMethod, id ).ToArray();

            //mark begin-end IndirectSpeech-subsents
            subsents.SetAttributeINDIRECTSPEECH_BEGINEND_4Subsents( directAndIndirectSpeechGlobalNumber );

            //add 2 result
            var v1 = from s1 in DSsubjectObjectsTuple.Subjects
                     from s2 in s1.SubjectEssences
                     select s2.VerbValue;
            var v2 = from s1 in subjects
                     from s2 in s1.SubjectEssences
                     select s2.VerbValue;
            //склеиваем название нескольких (к.п. 2-х) глаголов
            var verbsValues = v1.Concat( v2 ).Distinct().JoinBy( ", " );

            //создем новые пары субъект высказывания Entity - название нескольких (к.п. 2-х) глаголов Verb
            var se = from s1 in DSsubjectObjectsTuple.Subjects
                     from s2 in s1.SubjectEssences
                     select new SubjectEssence( s2, verbsValues );

            //создем новый IEnumerable< SubjectData >
            subjects = new SubjectData
                       ( 
                           se.ToList(), 
                           DSsubjectObjectsTuple.Subjects.First().LeftExtremeElement, 
                           subjects.Last().RightExtremeElement 
                       )
                       .RepeatOnce();
            //создаем объединенный результат some-rule-DS + IS_ParentChild_DanglingParticipleAfterDS_01
            return (new SubjectObjectsTuple( subjects,
                                             DSsubjectObjectsTuple.Objects.Select( o => o.Entity ).Concat( objects ), 
                                             DSsubjectObjectsTuple.RuleId + " + " + Id ));
        #endregion
        }
    }
    //===================================================//

    internal class IS_ParentChild_DanglingParticipleAfterDS_01 : IS_ParentChild_DanglingParticipleAfterDSBase
    {
        public IS_ParentChild_DanglingParticipleAfterDS_01() : base()
        {
        }

        private static List< SubjectAndAnaphoriaEntityTuple > GetSubjects( IEnumerable< XElement > sequence )
        {
            const string ENTITY_MI = "N";

            var sewra = default(SubjectAndAnaphoriaEntityTuple);
            var list  = new List< SubjectAndAnaphoriaEntityTuple >();
            foreach ( var e in sequence )
            {
                if ( e.IsDirectSubjectEntity( ENTITY_MI ) )
                    list.Add( new SubjectAndAnaphoriaEntityTuple( e ) );
                else if ( (sewra = e.ResolveAnaphoria4SubjectEntity( ENTITY_MI )) != null )
                    list.Add( sewra );
            }
            return (list);
        }

        private static XElement GetVerb( IEnumerable< XElement > sequence, Language language )
        {
            const string VERB_MI = "A";
            const DictionaryTypeVerbs dictionaryType = DictionaryTypeVerbs.VerbsOpinion2;

            Func< XElement, bool > verbConditionFunction = 
                v => language.DictionaryManager.ExistsInVerbs( v.GetAttributeVALUE().ToLowerEx(), dictionaryType );

            var verb = sequence.FindVERBtypeDEEPR( verbConditionFunction, VERB_MI );
            return (verb);
        }

        protected override bool IsParentMatchCondition( SubsentTuple tuple, Language language, out IEnumerable< SubjectData > subjectData )
        {            
            subjectData = null;            

            if ( tuple.CHILD_DP.IsSubsentHasSubject() )
                return (false);
            var verb = GetVerb( tuple.CHILD_DP.ElementsVerb(), language );
            if ( verb.IsNull() )
                return (false);

            var elements = tuple.CHILD_DP.ElementsBeforeSelf();
            //if ( elements.IsHasSubject() )
              //  return (false);
            var subjects = GetSubjects( elements );            
            if ( !subjects.AnyEx() )
                return (false);


            var subjectEssences = new List< SubjectEssence >();
            foreach ( var s in subjects )
                subjectEssences.Add( new SubjectEssence( s, verb ) );

            var sd = new SubjectData( subjectEssences, subjects.First().Entity, verb );
            subjectData = sd.RepeatOnce();
            return (true);

        }
        protected override bool IsChildMatchCondition( SubsentTuple tuple, Language language )
        {
            return (inderectspeech3.IsCondition( tuple.CHILD, language ));
        }
    }
    //===================================================//

    internal class IS_ParentChild_DanglingParticipleAfterDSGroup : IS_GroupBase< IS_ParentChild_DanglingParticipleAfterDSBase, SubsentTuple >
    {
        public static IS_ParentChild_DanglingParticipleAfterDSBase[] Get()
        {
            return
            (
                new IS_ParentChild_DanglingParticipleAfterDSBase[]
                { 
                    new IS_ParentChild_DanglingParticipleAfterDS_01(),
                }
            );
        }

        private IS_ParentChild_DanglingParticipleAfterDSGroup() : base( Get() )
        {
        }        
        static IS_ParentChild_DanglingParticipleAfterDSGroup()
        {
            Instance = new IS_ParentChild_DanglingParticipleAfterDSGroup();
        }
        public static IS_ParentChild_DanglingParticipleAfterDSGroup Instance
        {
            get;
            private set;
        }

        protected override SubsentTuple[] GetEssence4Processing( XElement sent )
        {
            return (IS_ParentChild_DanglingParticipleAfterDSBase.GetSubsentTuples( sent ).ToArray());
        }
        public SubjectObjectsTuple Process( XElement sent, Language language, int directAndIndirectSpeechGlobalNumber, ObjectAllocateMethod objectAllocateMethod, SubjectObjectsTuple DSsubjectObjectsTuple )
        {
            var essences = GetEssence4Processing( sent );
            foreach ( var essence in essences )
            {
                //if ( essence.IsNotHasSubject() )
                //{
                foreach ( var rule in _Rules )
                {
                    var sot = rule.Process( essence, language, directAndIndirectSpeechGlobalNumber, objectAllocateMethod, DSsubjectObjectsTuple );
                    if ( sot.IsNotNull() )
                    {
                        return (sot);
                    }
                }
                //}
            }
            return (null);
        }

        protected override SubjectObjectsTuple ProcessEssence( IS_ParentChild_DanglingParticipleAfterDSBase rule, SubsentTuple essence, Language language
            , ref int directAndIndirectSpeechGlobalNumber, ObjectAllocateMethod objectAllocateMethod )
        {
            throw (new InvalidOperationException());
        }
    }

}
