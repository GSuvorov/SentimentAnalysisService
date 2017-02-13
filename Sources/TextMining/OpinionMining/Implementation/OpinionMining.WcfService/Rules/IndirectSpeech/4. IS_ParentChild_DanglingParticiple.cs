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

    internal abstract class IS_ParentChild_DanglingParticipleBase : RuleBase
    {
        public IS_ParentChild_DanglingParticipleBase( string id = null ) : base( id )
        {
        }

        internal class SubsentTuple
        {
            public SubsentTuple( XElement parent, XElement child_dp, XElement child )
            {
                parent  .ThrowIfNotSubsent();
                child   .ThrowIfNotSubsent();
                child_dp.ThrowIfNotSubsent();

                PARENT   = parent;
                CHILD_DP = child_dp;
                CHILD    = child;
            }
            public XElement PARENT
            {
                get;
                private set;
            }
            public XElement CHILD_DP
            {
                get;
                private set;
            }
            public XElement CHILD
            {
                get;
                private set;
            }
            public bool IsNotHasSubject()
            {
                return (PARENT.IsSubsentNotHasSubject() && CHILD_DP.IsSubsentNotHasSubject() && CHILD.IsSubsentNotHasSubject());
            }
        }

        public static IEnumerable< SubsentTuple > GetSubsentTuples( XElement sent )
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

                         where ( parent  .IsSubsentNotHasSubject() && 
                                 child_dp.IsSubsentNotHasSubject() && 
                                 child   .IsSubsentNotHasSubject() )

                         select new SubsentTuple( parent, child_dp, child );

            return (tuples);
        }

        protected abstract bool IsParentMatchCondition( SubsentTuple tuple, Language language, out IEnumerable< SubjectData > subjectData );
        protected abstract bool IsChildMatchCondition( SubsentTuple tuple, Language language );

        public virtual SubjectObjectsTuple Process( SubsentTuple tuple, Language language
            , ref int directAndIndirectSpeechGlobalNumber, ObjectAllocateMethod objectAllocateMethod )
        {
        #region [.condition.]
            var subjects = default(IEnumerable< SubjectData >);
            if ( !IsParentMatchCondition( tuple, language, out subjects ) || 
                 !IsChildMatchCondition( tuple, language ) )
            {
                return (null); 
            }
        #endregion

        #region [.match condition.]
            MatchActionDebugInfoOutput( this.Id );

            //growup global IndirectSpeech-subsent number
            directAndIndirectSpeechGlobalNumber++;

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
            return (new SubjectObjectsTuple( subjects, objects, Id ));
        #endregion
        }
    }
    //===================================================//

    internal class IS_ParentChild_DanglingParticiple_01 : IS_ParentChild_DanglingParticipleBase
    {
        public IS_ParentChild_DanglingParticiple_01() : base()
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
            if ( elements.IsHasSubject() )
                return (false);
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

    internal class IS_ParentChild_DanglingParticipleGroup : IS_GroupBase< IS_ParentChild_DanglingParticipleBase, IS_ParentChild_DanglingParticipleBase.SubsentTuple >
    {
        public static IS_ParentChild_DanglingParticipleBase[] Get()
        {
            return
            (
                new IS_ParentChild_DanglingParticipleBase[]
                { 
                    new IS_ParentChild_DanglingParticiple_01(),
                }
            );
        }

        public IS_ParentChild_DanglingParticipleGroup() : base( Get() )
        {
        }

        protected override IS_ParentChild_DanglingParticipleBase.SubsentTuple[] GetEssence4Processing( XElement sent )
        {
            return (IS_ParentChild_DanglingParticipleBase.GetSubsentTuples( sent ).ToArray());
        }
        protected override SubjectObjectsTuple ProcessEssence( IS_ParentChild_DanglingParticipleBase pattern, IS_ParentChild_DanglingParticipleBase.SubsentTuple essence, Language language
            , ref int directAndIndirectSpeechGlobalNumber, ObjectAllocateMethod objectAllocateMethod )
        {
            if ( essence.IsNotHasSubject() )
            {
                return (pattern.Process( essence, language, ref directAndIndirectSpeechGlobalNumber, objectAllocateMethod ));
            }
            return (null);
        }        
    }
}
