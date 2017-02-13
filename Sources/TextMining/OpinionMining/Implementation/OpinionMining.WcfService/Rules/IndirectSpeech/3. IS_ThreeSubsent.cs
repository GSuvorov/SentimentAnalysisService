using System;
using System.Collections.Generic;
using System.Linq;
using System.Xml;
using System.Xml.Linq;

namespace OpinionMining
{
    using OpinionMining.DirectIndirectSpeechExtensions;
    using TextMining.Core;

    internal abstract class IS_ThreeSubsentBase : RuleBase
    {
        public IS_ThreeSubsentBase( string id = null ) : base( id )
        {
        }

        internal class SubsentTuple
        {
            public SubsentTuple( XElement subsent1, XElement subsent2, XElement subsent3 )
            {
                subsent1.ThrowIfNotSubsent();
                subsent2.ThrowIfNotSubsent();
                subsent3.ThrowIfNotSubsent();

                SUBSENT1 = subsent1;
                SUBSENT2 = subsent2;
                SUBSENT3 = subsent3;
            }
            public XElement SUBSENT1
            {
                get;
                private set;
            }
            public XElement SUBSENT2
            {
                get;
                private set;
            }
            public XElement SUBSENT3
            {
                get;
                private set;
            }
            public bool IsNotHasSubject()
            {
                return (SUBSENT1.IsSubsentNotHasSubject() && SUBSENT2.IsSubsentNotHasSubject() && SUBSENT3.IsSubsentNotHasSubject());
            }
        }

        public static IEnumerable< SubsentTuple > GetSubsentTuples( XElement sent )
        {
            sent.ThrowIfNotSent();

            var tuples = from subsent1 in sent.DescendantsSubsentence()
                         where subsent1.AttributeTypeAreEqual( SubsentenceType.Subordinate,
                                                               SubsentenceType.Default, 
                                                               SubsentenceType.Introductory )

                         let subsent2 = subsent1.ElementsAfterSelfExceptT().FirstOrDefault()
                         where ( subsent2.IsNotNull() && 
                                 subsent2.IsElementSUBSENT() && 
                                 subsent2.AttributeTypeAreEqual( SubsentenceType.Subordinate,
                                                                 SubsentenceType.Default, 
                                                                 SubsentenceType.Introductory )
                               )

                         let subsent3 = subsent2.ElementsAfterSelfExceptT().FirstOrDefault()
                         where ( subsent3.IsNotNull() &&
                                 subsent3.IsElementSUBSENT() && 
                                 subsent3.AttributeTypeAreEqual( SubsentenceType.Subordinate,
                                                                 SubsentenceType.Default, 
                                                                 SubsentenceType.Introductory ) )

                         where ( subsent1.IsSubsentNotHasSubject()  && 
                                 subsent2.IsSubsentNotHasSubject()  && 
                                 subsent3.IsSubsentNotHasSubject() )

                         select new SubsentTuple( subsent1, subsent2, subsent3 );

            return (tuples);
        }

        protected abstract bool IsSubsent1MatchCondition( SubsentTuple tuple, Language language );
        protected abstract bool IsSubsent2MatchCondition( SubsentTuple tuple, Language language, out IEnumerable< SubjectData > subjectData );

        public virtual SubjectObjectsTuple Process( SubsentTuple tuple, Language language
            , ref int directAndIndirectSpeechGlobalNumber, ObjectAllocateMethod objectAllocateMethod )
        {
        #region [.condition.]
            var subjects = default(IEnumerable< SubjectData >);
            if ( !IsSubsent2MatchCondition( tuple, language, out subjects ) )
            {
                return (null); 
            }
        #endregion

        #region [.match condition.]
            //allocate objects
            var subsents1 = tuple.SUBSENT1.BeforeSubsents().ToArray();            
            if ( subjects.HasAnySubjectEntityAncestorInAnySubsents( subsents1 ) )
                return (null);

            var subsents3 = tuple.SUBSENT3.AfterAndSelfSubsents().ToArray();
            if ( subjects.HasAnySubjectEntityAncestorInAnySubsents( subsents3 ) )
                return (null);

            MatchActionDebugInfoOutput( this.Id );

            //growup global IndirectSpeech-subsent number
            directAndIndirectSpeechGlobalNumber++;

            //set 'ISSUBJECT' attribute for subject-entity
            subjects.SetAttributeISSUBJECTforAllEntity( directAndIndirectSpeechGlobalNumber );

            //set 'ISOBJECT' attribute for all object-entity
            var id = subjects.GetAttributeIDForce();

            var objects1 = subsents1.TryAllocateObjectsInSubsents( objectAllocateMethod, id ).ToArray();
            //mark begin-end IndirectSpeech-subsents-1
            subsents1.SetAttributeINDIRECTSPEECH_BEGINEND_4Subsents( directAndIndirectSpeechGlobalNumber );

            var objects3 = subsents3.TryAllocateObjectsInSubsents( objectAllocateMethod, id ).ToArray();
            //mark begin-end IndirectSpeech-subsents-3
            subsents3.SetAttributeINDIRECTSPEECH_BEGINEND_4Subsents( directAndIndirectSpeechGlobalNumber );

            var u = tuple.SUBSENT1.ElementsExceptT().LastOrDefault();
            var elements1 = tuple.SUBSENT1.Elements().TakeWhile( e => e != u );
            var elements1objects = elements1.TryAllocateObjects( objectAllocateMethod, id ).ToArray();
            //mark begin-end IndirectSpeech-subsents-1
            elements1.SetAttributeINDIRECTSPEECH_BEGINEND( directAndIndirectSpeechGlobalNumber );

            //build all objects
            var objects = objects1
                         .Concat
                         (
                          elements1objects
                         )
                         .Concat
                         (
                          objects3
                         )
                         .ToArray();

            //add 2 result
            return (new SubjectObjectsTuple( subjects, objects, Id ));
        #endregion
        }
    }
    //===================================================//

    internal class IS_ThreeSubsent_01 : IS_ThreeSubsentBase
    {
        public IS_ThreeSubsent_01(): base()
        {
        }

        protected override bool IsSubsent1MatchCondition( SubsentTuple tuple, Language language )
        {
            return (inderectspeech2.IsCondition( tuple.SUBSENT1 ));
        }
        protected override bool IsSubsent2MatchCondition( SubsentTuple tuple, Language language, out IEnumerable< SubjectData > subjectData )
        {            
            subjectData = null;

            var subsent    = tuple.SUBSENT2;
            var resultList = default(List< SubjectData >);
            var sd         = default(SubjectData);

            sd = pattern1.GetSubjectData( subsent, language );                    if ( sd.IsNotNull() ) goto NEXT;
            sd = pattern2.GetSubjectData( subsent, language );                    if ( sd.IsNotNull() ) goto NEXT;
            sd = pattern3_pointview.Instance.GetSubjectData( subsent           ); if ( sd.IsNotNull() ) goto NEXT;
            sd = pattern4_opinion  .Instance.GetSubjectData( subsent, language ); if ( sd.IsNotNull() ) goto NEXT;
            sd = pattern5_onglance .Instance.GetSubjectData( subsent           ); if ( sd.IsNotNull() ) goto NEXT;
            sd = pattern23.GetSubjectData( subsent, language );                   if ( sd.IsNotNull() ) goto NEXT;
            sd = pattern24.GetSubjectData( subsent, language );                   if ( sd.IsNotNull() ) goto NEXT;

        NEXT:
            sd.Add2List( ref resultList );

            if ( resultList.AnyEx() )
            {
                resultList = resultList.MakeDistinct();

                subjectData = resultList;
                return (true);
            }

            return (false);
        }
    }
    //===================================================//

    internal class IS_ThreeSubsentGroup : IS_GroupBase< IS_ThreeSubsentBase, IS_ThreeSubsentBase.SubsentTuple >
    {
        private static IS_ThreeSubsentBase[] Get()
        {
            return
            (
                new IS_ThreeSubsentBase[]
                { 
                    new IS_ThreeSubsent_01(),
                }
            );
        }

        public IS_ThreeSubsentGroup() : base( Get() )
        {
        }

        protected override IS_ThreeSubsentBase.SubsentTuple[] GetEssence4Processing( XElement sent )
        {
            return (IS_ThreeSubsentBase.GetSubsentTuples( sent ).ToArray());
        }
        protected override SubjectObjectsTuple ProcessEssence( IS_ThreeSubsentBase pattern, IS_ThreeSubsentBase.SubsentTuple essence, Language language
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
