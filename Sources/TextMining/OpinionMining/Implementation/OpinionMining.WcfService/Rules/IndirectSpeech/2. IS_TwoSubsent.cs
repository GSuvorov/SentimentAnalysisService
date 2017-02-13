using System;
using System.Collections.Generic;
using System.Linq;
using System.Xml;
using System.Xml.Linq;

namespace OpinionMining
{
    using OpinionMining.DirectIndirectSpeechExtensions;
    using TextMining.Core;

    internal abstract class IS_TwoSubsentBase : RuleBase
    {
        public IS_TwoSubsentBase( string id = null ) : base( id )
        {
        }

        internal class SubsentPair
        {
            public SubsentPair( XElement subsent1, XElement subsent2 )
            {
                subsent1.ThrowIfNotSubsent();
                subsent2.ThrowIfNotSubsent();

                SUBSENT1 = subsent1;
                SUBSENT2 = subsent2;
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

            public SubsentPair SwapSubsents()
            {
                return (new SubsentPair( SUBSENT2, SUBSENT1 ));
            }
            public bool IsNotHasSubject()
            {
                return (SUBSENT1.IsSubsentNotHasSubject() && SUBSENT2.IsSubsentNotHasSubject());
            }
        }

        public static IEnumerable< SubsentPair > GetSubsentPairs( XElement sent )
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
                                                                 SubsentenceType.Introductory ) )

                         where ( subsent1.IsSubsentNotHasSubject() && 
                                 subsent2.IsSubsentNotHasSubject() )

                         select new SubsentPair( subsent1, subsent2 );
            return (tuples);
        }

        protected bool SubsentsIsSwaped
        {
            get;
            set;
        }
        protected XElement GetInderectspeechSubsent( SubsentPair pair )
        {
            var subsent = (SubsentsIsSwaped ? pair.SUBSENT2 : pair.SUBSENT1);
            return (subsent);
        }

        protected abstract bool IsSubsent1MatchCondition( SubsentPair pair, Language language, out IEnumerable< SubjectData > subjectData );
        protected abstract bool IsSubsent2MatchCondition( SubsentPair pair, Language language );

        public virtual SubjectObjectsTuple Process( SubsentPair pair, Language language
            , ref int directAndIndirectSpeechGlobalNumber, ObjectAllocateMethod objectAllocateMethod )
        {
        #region [.condition.]
            SubsentsIsSwaped = false;

            var subjects = default(IEnumerable< SubjectData >);
            if ( !IsSubsent2MatchCondition( pair, language ) ||
                 !IsSubsent1MatchCondition( pair, language, out subjects ) )
            {
                pair = pair.SwapSubsents();
                SubsentsIsSwaped = true;
                if ( !IsSubsent2MatchCondition( pair, language ) ||
                     !IsSubsent1MatchCondition( pair, language, out subjects ) )
                {
                    return (null); 
                }
            }
        #endregion

        #region [.match condition.]
            //allocate objects
            var subsents = (SubsentsIsSwaped ? pair.SUBSENT2.BeforeAndSelfSubsents() :
                                               pair.SUBSENT2.AfterAndSelfSubsents()
                           ).ToArray();

            if ( subjects.HasAnySubjectEntityAncestorInAnySubsents( subsents ) )
                return (null);

            MatchActionDebugInfoOutput( this.Id );

            //growup global IndirectSpeech-subsent number
            directAndIndirectSpeechGlobalNumber++;

            //set 'ISSUBJECT' attribute for subject-entity
            subjects.SetAttributeISSUBJECTforAllEntity( directAndIndirectSpeechGlobalNumber );

            //set 'ISOBJECT' attribute for all object-entity
            var id = subjects.GetAttributeIDForce();

            var objects = subsents.TryAllocateObjectsInSubsents( objectAllocateMethod, id ).ToArray();

            //mark begin-end IndirectSpeech-subsents
            subsents.SetAttributeINDIRECTSPEECH_BEGINEND_4Subsents( directAndIndirectSpeechGlobalNumber );

            //add 2 result
            return (new SubjectObjectsTuple( subjects, objects, Id ));
        #endregion
        }
    }
    //===================================================//

    internal class IS_TwoSubsent_02 : IS_TwoSubsentBase
    {
        public IS_TwoSubsent_02() : base()
        {
        }

        protected override bool IsSubsent1MatchCondition( SubsentPair pair, Language language, out IEnumerable< SubjectData > subjectData )
        {
            throw new InvalidOperationException();
        }
        protected override bool IsSubsent2MatchCondition( SubsentPair pair, Language language )
        {
            throw new InvalidOperationException();
        }

        public override SubjectObjectsTuple Process( SubsentPair pair, Language language
            , ref int directAndIndirectSpeechGlobalNumber, ObjectAllocateMethod objectAllocateMethod )
        {
        #region [.condition.]
            SubjectData sd = null;

            var u = inderectspeech1.GetElementUByCondition( pair.SUBSENT1.Elements() );
            if ( u.IsNull() )
                return (null);

            sd = pattern1.GetSubjectData( u.ElementsAfterSelf(), language );
            if ( sd.IsNull() )
                return (null);

            var u2 = inderectspeech1.GetElementUByCondition( sd.RightExtremeElement.ElementsAfterSelf() );
            if ( u2.IsNull() )
                return (null);
        #endregion

        #region [.match condition.]
            //allocate objects
            var subsents2 = (SubsentsIsSwaped ? pair.SUBSENT2.BeforeAndSelfSubsents() : 
                                                pair.SUBSENT2.AfterAndSelfSubsents()                                                
                           ).ToArray();

            if ( sd.HasAnySubjectEntityAncestorInAnySubsents( subsents2 ) )
                return (null);

            MatchActionDebugInfoOutput( this.Id );

            //growup global IndirectSpeech-subsent number
            directAndIndirectSpeechGlobalNumber++;

            //set 'ISSUBJECT' attribute for subject-entity
            sd.SetAttributeISSUBJECTforAllEntity( directAndIndirectSpeechGlobalNumber );

            //set 'ISOBJECT' attribute for all object-entity
            var id = sd.FirstEntity().GetAttributeIDForce();

            //allocate objects in IndirectSpeech-subsents-2
            var objects2 = subsents2.TryAllocateObjectsInSubsents( objectAllocateMethod, id ).ToArray();

            //mark begin-end IndirectSpeech-subsents-2
            subsents2.SetAttributeINDIRECTSPEECH_BEGINEND_4Subsents( directAndIndirectSpeechGlobalNumber );

            var objects1 = pair.SUBSENT1.ElementsExceptT().TakeWhile( e => e != u ).TryAllocateObjects( objectAllocateMethod, id ).ToArray();
            //mark begin-end IndirectSpeech-subsent-1
            pair.SUBSENT1.Elements().TakeWhile( e => e != u ).SetAttributeINDIRECTSPEECH_BEGINEND( directAndIndirectSpeechGlobalNumber );

            //build all objects
            var objects = objects1
                         .Concat
                         (
                          objects2
                         )
                         .ToArray();

            //add 2 result
            return (new SubjectObjectsTuple( sd.RepeatOnce(), objects, Id ));
        #endregion
        }
    }

    internal class IS_TwoSubsent_01 : IS_TwoSubsentBase
    {
        public IS_TwoSubsent_01() : base()
        {
        }
        
        protected override bool IsSubsent1MatchCondition( SubsentPair pair, Language language, out IEnumerable< SubjectData > subjectData )
        {
            subjectData = null;

            var subsent    = pair.SUBSENT1;
            var resultList = default(List< SubjectData >);
            var sd         = default(SubjectData);

            sd = pattern1.GetSubjectData( subsent, language ); if ( sd.IsNotNull() ) goto NEXT;
            sd = pattern2.GetSubjectData( subsent, language ); if ( sd.IsNotNull() ) goto NEXT;
            sd = pattern3_pointview.Instance.GetSubjectData( subsent           ); if ( sd.IsNotNull() ) goto NEXT;
            sd = pattern4_opinion  .Instance.GetSubjectData( subsent, language ); if ( sd.IsNotNull() ) goto NEXT;
            sd = pattern5_onglance .Instance.GetSubjectData( subsent           ); if ( sd.IsNotNull() ) goto NEXT;
            //sd = pattern7 .GetSubjectData( subsent, language ); if ( sd.IsNotNull() ) goto NEXT;
            sd = pattern18.GetSubjectData( subsent, language ); if ( sd.IsNotNull() ) goto NEXT;
            sd = pattern19.GetSubjectData( subsent, language ); if ( sd.IsNotNull() ) goto NEXT;
            sd = pattern20.GetSubjectData( subsent, language ); if ( sd.IsNotNull() ) goto NEXT;
            sd = pattern21.GetSubjectData( subsent, language ); if ( sd.IsNotNull() ) goto NEXT;
            sd = pattern22.GetSubjectData( subsent, language ); if ( sd.IsNotNull() ) goto NEXT;
            sd = pattern23.GetSubjectData( subsent, language ); if ( sd.IsNotNull() ) goto NEXT;

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
        protected override bool IsSubsent2MatchCondition( SubsentPair pair, Language language )
        {
            var subsent = GetInderectspeechSubsent( pair );
            return (inderectspeech2.IsCondition( subsent ));
        }
    }

    internal class IS_TwoSubsent_03 : IS_TwoSubsentBase
    {
        public IS_TwoSubsent_03() : base()
        {
        }

        protected override bool IsSubsent1MatchCondition( SubsentPair pair, Language language, out IEnumerable< SubjectData > subjectData )
        {
            subjectData = null;

            var subsent    = pair.SUBSENT1;
            var resultList = default(List< SubjectData >);
            var sd         = default(SubjectData);

            sd = pattern8 .GetSubjectData( subsent, language ); if ( sd.IsNotNull() ) goto NEXT;
            sd = pattern9 .GetSubjectData( subsent, language ); if ( sd.IsNotNull() ) goto NEXT;
            sd = pattern10.GetSubjectData( subsent, language ); if ( sd.IsNotNull() ) goto NEXT;
            sd = pattern11.GetSubjectData( subsent, language ); if ( sd.IsNotNull() ) goto NEXT;

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
        protected override bool IsSubsent2MatchCondition( SubsentPair pair, Language language )
        {
            var subsent = GetInderectspeechSubsent( pair );
            return (inderectspeech3_inderectspeech2.IsCondition( pair.SUBSENT2, subsent, language ));
        }
    }

    internal class IS_TwoSubsent_04 : IS_TwoSubsent_03
    {
        public IS_TwoSubsent_04() : base()
        {
        }

        protected override bool IsSubsent2MatchCondition( SubsentPair pair, Language language )
        {
            var subsent = GetInderectspeechSubsent( pair );
            return (inderectspeech4_inderectspeech2.IsCondition( pair.SUBSENT2, subsent, language ));
        }
    }

    internal class IS_TwoSubsent_05 : IS_TwoSubsent_03
    {
        public IS_TwoSubsent_05() : base()
        {
        }

        protected override bool IsSubsent2MatchCondition( SubsentPair pair, Language language )
        {
            var subsent = GetInderectspeechSubsent( pair );
            return (inderectspeech5_inderectspeech2.IsCondition( pair.SUBSENT2, subsent, language ));
        }
    }

    internal class IS_TwoSubsent_06 : IS_TwoSubsent_03
    {
        public IS_TwoSubsent_06() : base()
        {
        }

        protected override bool IsSubsent2MatchCondition( SubsentPair pair, Language language )
        {
            var subsent = GetInderectspeechSubsent( pair );
            return (inderectspeech6_inderectspeech2.IsCondition( pair.SUBSENT2, subsent, language ));
        }
    }

    internal class IS_TwoSubsent_07 : IS_TwoSubsentBase
    {
        public IS_TwoSubsent_07() : base()
        {
        }

        protected override bool IsSubsent1MatchCondition( SubsentPair pair, Language language, out IEnumerable< SubjectData > subjectData )
        {
            //if ( SubsentsIsSwaped )
            //{
                var subsent = pair.SUBSENT1;

                var sd = pattern7.GetSubjectData( subsent, language );
                if ( sd.IsNotNull() )
                {
                    subjectData = sd.RepeatOnce();
                    return (true);
                }
            //}
            subjectData = null;
            return (false);
        }
        protected override bool IsSubsent2MatchCondition( SubsentPair pair, Language language )
        {
            return (/*SubsentsIsSwaped &&*/ inderectspeech2.IsCondition( pair.SUBSENT2 ));
        }
    }
    //===================================================//

    internal class IS_TwoSubsentGroup : IS_GroupBase< IS_TwoSubsentBase, IS_TwoSubsentBase.SubsentPair >
    {
        private static IS_TwoSubsentBase[] Get()
        {
            return
            (
                new IS_TwoSubsentBase[]
                { 
                    new IS_TwoSubsent_02(),
                    new IS_TwoSubsent_01(),
                    new IS_TwoSubsent_03(),
                    new IS_TwoSubsent_04(),
                    new IS_TwoSubsent_05(),
                    new IS_TwoSubsent_06(),
                    new IS_TwoSubsent_07(),
                }
            );
        }

        public IS_TwoSubsentGroup() : base( Get() )
        {
        }

        protected override IS_TwoSubsentBase.SubsentPair[] GetEssence4Processing( XElement sent )
        {
            return (IS_TwoSubsentBase.GetSubsentPairs( sent ).ToArray());
        }
        protected override SubjectObjectsTuple ProcessEssence( IS_TwoSubsentBase pattern, IS_TwoSubsentBase.SubsentPair essence, Language language
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
