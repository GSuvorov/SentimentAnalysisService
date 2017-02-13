using System;
using System.Collections.Generic;
using System.Linq;
using System.Xml;
using System.Xml.Linq;

namespace OpinionMining
{
    using OpinionMining.DirectIndirectSpeechExtensions;
    using TextMining.Core;

    internal abstract class IS_ParentChild_DescBase : RuleBase
    {
        public IS_ParentChild_DescBase( string id = null ) : base( id )
        {
        }

        internal class SubsentPair
        {
            public SubsentPair( XElement parent, XElement child )
            {
                parent.ThrowIfNotSubsent();
                child.ThrowIfNotSubsent();

                PARENT = parent;
                CHILD = child;
            }
            public XElement PARENT
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
                return (PARENT.IsSubsentNotHasSubject() && CHILD.IsSubsentNotHasSubject());
            }
        }

        public static IEnumerable< SubsentPair > GetSubsentPairs( XElement sent )
        {
            sent.ThrowIfNotSent();

            var tuples = from parent in sent.DescendantsSubsentence()
                         where parent.AttributeTypeAreEqual( SubsentenceType.Subordinate,
                                                             SubsentenceType.Default, 
                                                             SubsentenceType.Introductory )

                         from child in parent.ElementsSubsentence()
                         where child.AttributeTypeAreEqual( SubsentenceType.Subordinate,
                                                            SubsentenceType.Default, 
                                                            SubsentenceType.Introductory )

                         where ( parent.IsSubsentNotHasSubject()  && 
                                 child .IsSubsentNotHasSubject() )

                         select new SubsentPair( parent, child );

            return (tuples);
        }

        protected virtual bool IsParentMatchCondition( SubsentPair pair, Language language, out IEnumerable< SubjectData > subjectData )
        {
            subjectData = null;

            var resultList = default(List< SubjectData >);
            var sd         = default(SubjectData);

            var elementsAfterCHILD = pair.CHILD.ElementsAfterSelf().ToArray();

            sd = pattern7 .GetSubjectData( elementsAfterCHILD, language, pair.CHILD ); if ( sd.IsNotNull() ) goto NEXT;
            sd = pattern8 .GetSubjectData( elementsAfterCHILD, language, pair.CHILD ); if ( sd.IsNotNull() ) goto NEXT;
            sd = pattern9 .GetSubjectData( elementsAfterCHILD, language, pair.CHILD ); if ( sd.IsNotNull() ) goto NEXT;
            sd = pattern10.GetSubjectData( elementsAfterCHILD, language, pair.CHILD ); if ( sd.IsNotNull() ) goto NEXT;
            sd = pattern11.GetSubjectData( elementsAfterCHILD, language, pair.CHILD ); if ( sd.IsNotNull() ) goto NEXT;

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
        protected abstract bool IsChildMatchCondition( SubsentPair pair, Language language );

        public virtual SubjectObjectsTuple Process( SubsentPair pair, Language language
            , ref int directAndIndirectSpeechGlobalNumber, ObjectAllocateMethod objectAllocateMethod )
        {
        #region [.condition.]
            var subjects = default(IEnumerable< SubjectData >);
            if ( !IsParentMatchCondition( pair, language, out subjects ) || 
                 !IsChildMatchCondition( pair, language ) )
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
            var objects = pair.CHILD.ElementsExceptT().TryAllocateObjects( objectAllocateMethod, id );

            //mark begin-end IndirectSpeech-subsents
            var u = pair.CHILD.ElementsExceptT().LastOrDefault();
            pair.CHILD.Elements().TakeWhile( e => e != u ).SetAttributeINDIRECTSPEECH_BEGINEND( directAndIndirectSpeechGlobalNumber );

            //add 2 result
            return (new SubjectObjectsTuple( subjects, objects, Id ));
        #endregion
        }
    }
    //===================================================//

    internal class IS_ParentChild_Desc_01 : IS_ParentChild_DescBase
    {
        public IS_ParentChild_Desc_01() : base()
        {
        }

        protected override bool IsParentMatchCondition( SubsentPair pair, Language language, out IEnumerable< SubjectData > subjectData )
        {
            subjectData = null;

            var resultList = default(List< SubjectData >);
            var sd         = default(SubjectData);

            var elementsAfterCHILD = pair.CHILD.ElementsAfterSelf().ToArray();

            sd = pattern7 .GetSubjectData( elementsAfterCHILD, language, pair.CHILD ); if ( sd.IsNotNull() ) goto NEXT;
            sd = pattern8 .GetSubjectData( elementsAfterCHILD, language, pair.CHILD ); if ( sd.IsNotNull() ) goto NEXT;
            sd = pattern9 .GetSubjectData( elementsAfterCHILD, language, pair.CHILD ); if ( sd.IsNotNull() ) goto NEXT;
            sd = pattern10.GetSubjectData( elementsAfterCHILD, language, pair.CHILD ); if ( sd.IsNotNull() ) goto NEXT;
            sd = pattern11.GetSubjectData( elementsAfterCHILD, language, pair.CHILD ); if ( sd.IsNotNull() ) goto NEXT;
            sd = pattern18.GetSubjectData( elementsAfterCHILD, language, pair.CHILD ); if ( sd.IsNotNull() ) goto NEXT;
            sd = pattern19.GetSubjectData( elementsAfterCHILD, language, pair.CHILD ); if ( sd.IsNotNull() ) goto NEXT;
            sd = pattern20.GetSubjectData( elementsAfterCHILD, language, pair.CHILD ); if ( sd.IsNotNull() ) goto NEXT;
            sd = pattern21.GetSubjectData( elementsAfterCHILD, language, pair.CHILD ); if ( sd.IsNotNull() ) goto NEXT;
            sd = pattern22.GetSubjectData( elementsAfterCHILD, language, pair.CHILD ); if ( sd.IsNotNull() ) goto NEXT;
            sd = pattern23.GetSubjectData( elementsAfterCHILD, language, pair.CHILD ); if ( sd.IsNotNull() ) goto NEXT;
            sd = pattern24.GetSubjectData( elementsAfterCHILD, language, pair.CHILD ); if ( sd.IsNotNull() ) goto NEXT;

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
        protected override bool IsChildMatchCondition( SubsentPair pair, Language language )
        {
            return (inderectspeech3_inderectspeech2.IsCondition( pair.CHILD, language ));
        }
    }

    internal class IS_ParentChild_Desc_02 : IS_ParentChild_DescBase
    {
        public IS_ParentChild_Desc_02() : base()
        {
        }

        protected override bool IsChildMatchCondition( SubsentPair pair, Language language )
        {
            return (inderectspeech4_inderectspeech2.IsCondition( pair.CHILD, language ));
        }
    }

    internal class IS_ParentChild_Desc_03 : IS_ParentChild_DescBase
    {
        public IS_ParentChild_Desc_03() : base()
        {
        }

        protected override bool IsChildMatchCondition( SubsentPair pair, Language language )
        {
            return (inderectspeech5_inderectspeech2.IsCondition( pair.CHILD, language ));
        }
    }

    internal class IS_ParentChild_Desc_04 : IS_ParentChild_DescBase
    {
        public IS_ParentChild_Desc_04() : base()
        {
        }

        protected override bool IsChildMatchCondition( SubsentPair pair, Language language )
        {
            return (inderectspeech6_inderectspeech2.IsCondition( pair.CHILD, language ));
        }
    }

    internal class IS_ParentChild_Desc_05 : IS_ParentChild_DescBase
    {
        public IS_ParentChild_Desc_05() : base()
        {
        }

        protected override bool IsChildMatchCondition( SubsentPair pair, Language language )
        {
            throw (new InvalidOperationException());
        }

        private bool _IsChildMatchCondition( SubsentPair pair, Language language, out IEnumerable< SubjectData > subjectData )
        {
            subjectData = null;

            if ( !inderectspeech2.IsCondition( pair.CHILD ) )
                return (false);

            var resultList = default(List< SubjectData >);
            var sd         = default(SubjectData);
           
            sd = pattern1 .GetSubjectData( pair.CHILD, language ); if ( sd.IsNotNull() ) goto NEXT;
            sd = pattern2 .GetSubjectData( pair.CHILD, language ); if ( sd.IsNotNull() ) goto NEXT;
            sd = pattern6 .GetSubjectData( pair.CHILD, language ); if ( sd.IsNotNull() ) goto NEXT;
            sd = pattern3_pointview.Instance.GetSubjectData( pair.CHILD ); if ( sd.IsNotNull() ) goto NEXT;
            sd = pattern4_opinion  .Instance.GetSubjectData( pair.CHILD, language ); if ( sd.IsNotNull() ) goto NEXT;
            sd = pattern5_onglance .Instance.GetSubjectData( pair.CHILD ); if ( sd.IsNotNull() ) goto NEXT;

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
        public override SubjectObjectsTuple Process( SubsentPair pair, Language language
            , ref int directAndIndirectSpeechGlobalNumber, ObjectAllocateMethod objectAllocateMethod )
        {
        #region [.condition.]
            var subjects = default(IEnumerable< SubjectData >);
            if ( !_IsChildMatchCondition( pair, language, out subjects ) )
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
            var elements = pair.CHILD.ElementsAfterSelf();
            var subsents = pair.CHILD.AfterSubsents();
            var objects  = elements.TryAllocateObjects( objectAllocateMethod, id )
                          .Concat
                          ( 
                           subsents.TryAllocateObjectsInSubsents( objectAllocateMethod, id )
                          )
                          .ToArray();

            //mark begin-end IndirectSpeech-subsents
            subsents.SetAttributeINDIRECTSPEECH_BEGINEND_4Subsents( directAndIndirectSpeechGlobalNumber );
            elements.SetAttributeINDIRECTSPEECH_BEGINEND( directAndIndirectSpeechGlobalNumber );

            //add 2 result
            return (new SubjectObjectsTuple( subjects, objects, Id ));
        #endregion
        }
    }
    //===================================================//

    internal class IS_ParentChild_DescGroup : IS_GroupBase< IS_ParentChild_DescBase, IS_ParentChild_DescBase.SubsentPair >
    {
        public static IS_ParentChild_DescBase[] Get()
        {
            return
            (
                new IS_ParentChild_DescBase[]
                { 
                    new IS_ParentChild_Desc_05(),
                    new IS_ParentChild_Desc_01(),
                    new IS_ParentChild_Desc_02(),
                    new IS_ParentChild_Desc_03(),
                    new IS_ParentChild_Desc_04(),                    
                }
            );
        }

        public IS_ParentChild_DescGroup() : base( Get() )
        {
        }

        protected override IS_ParentChild_DescBase.SubsentPair[] GetEssence4Processing( XElement sent )
        {
            return (IS_ParentChild_DescBase.GetSubsentPairs( sent ).ToArray());
        }
        protected override SubjectObjectsTuple ProcessEssence( IS_ParentChild_DescBase pattern, IS_ParentChild_DescBase.SubsentPair essence, Language language
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
