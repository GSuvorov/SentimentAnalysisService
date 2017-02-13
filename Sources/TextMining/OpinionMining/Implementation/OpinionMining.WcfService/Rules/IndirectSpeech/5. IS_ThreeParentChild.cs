using System;
using System.Collections.Generic;
using System.Linq;
using System.Xml;
using System.Xml.Linq;

namespace OpinionMining
{
    using OpinionMining.DirectIndirectSpeechExtensions;
    using TextMining.Core;

    internal abstract class IS_ThreeParentChildBase : RuleBase
    {
        public IS_ThreeParentChildBase( string id = null ) : base( id )
        {
        }

        internal class SubsentTuple
        {
            public SubsentTuple( XElement parent, XElement child, XElement subsent3 )
            {
                parent  .ThrowIfNotSubsent();
                child   .ThrowIfNotSubsent();
                subsent3.ThrowIfNotSubsent();

                PARENT   = parent;
                CHILD    = child;
                SUBSENT3 = subsent3;
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
            public XElement SUBSENT3
            {
                get;
                private set;
            }
            public bool IsNotHasSubject()
            {
                return (PARENT.IsSubsentNotHasSubject() && CHILD.IsSubsentNotHasSubject() && SUBSENT3.IsSubsentNotHasSubject());
            }
        }

        public static IEnumerable< SubsentTuple > GetSubsentTuples( XElement sent )
        {
            sent.ThrowIfNotSent();

            var tuples = from parent in sent.DescendantsSubsentence()
                         where parent.AttributeTypeAreEqual( SubsentenceType.Subordinate,
                                                             SubsentenceType.Default, 
                                                             SubsentenceType.Introductory )
                               && parent.IsSubsentNotHasSubject() 

                         from child in parent.ElementsSubsentence()
                         where child.AttributeTypeAreEqual( SubsentenceType.Subordinate,
                                                            SubsentenceType.Default, 
                                                            SubsentenceType.Introductory )

                         where child.IsSubsentNotHasSubject()  

                         let subsent3 = parent.ElementsAfterSelfExceptT().FirstOrDefault()
                         where ( subsent3.IsNotNull() &&
                                 subsent3.IsElementSUBSENT() && 
                                 subsent3.AttributeTypeAreEqual( SubsentenceType.Subordinate,
                                                                 SubsentenceType.Default, 
                                                                 SubsentenceType.Introductory ) )
                               && subsent3.IsSubsentNotHasSubject()   

                         select new SubsentTuple( parent, child, subsent3 );

            return (tuples);
        }

        protected abstract bool IsParentMatchCondition( SubsentTuple tuple );
        protected abstract bool IsChildMatchCondition( SubsentTuple tuple, Language language, out IEnumerable< SubjectData > subjectTuple );        

        public virtual SubjectObjectsTuple Process( SubsentTuple tuple, Language language
            , ref int directAndIndirectSpeechGlobalNumber, ObjectAllocateMethod objectAllocateMethod )
        {
        #region [.condition.]
            var subjects = default(IEnumerable< SubjectData >);
            if ( !IsParentMatchCondition( tuple ) || 
                 !IsChildMatchCondition( tuple, language, out subjects ) )
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
            #region previous. commented
            /*
            //allocate objects
            var objects = tuple.PARENT.ElementsExceptT().TryAllocateObjects( id )
                         .Concat
                         (
                         tuple.SUBSENT3.ElementsExceptT().TryAllocateObjects( id )
                         );

            //create IndirectSpeech-parent-1
            var subsentIS = directAndIndirectSpeechGlobalNumber.CreateSubsentIndirectSpeech();
            subsentIS.MoveIn2ThisAllNodes( tuple.PARENT.Nodes().TakeWhile( n => n != tuple.CHILD ) );
            tuple.CHILD.AddBeforeSelf( subsentIS );

            //create IndirectSpeech-subsent-3
            subsentIS = directAndIndirectSpeechGlobalNumber.CreateSubsentIndirectSpeech();
            subsentIS.MoveIn2ThisAllNodes( tuple.SUBSENT3.Nodes() );
            tuple.SUBSENT3.Add( subsentIS );
            */
            #endregion

            //allocate objects
            var elements1 = tuple.PARENT.Elements().TakeWhile( e => e != tuple.CHILD );

            var subsents1 = tuple.PARENT.BeforeSubsents().ToArray();
            var subsents3 = tuple.SUBSENT3.AfterAndSelfSubsents() .ToArray();

            var objects = subsents1.TryAllocateObjectsInSubsents( objectAllocateMethod, id )
                         .Concat
                         (
                         elements1.TryAllocateObjects( objectAllocateMethod, id )
                         )
                         .Concat
                         (
                          subsents3.TryAllocateObjectsInSubsents( objectAllocateMethod, id )
                         )
                         .ToArray();

            //mark begin-end IndirectSpeech-subsents-1
            subsents1.SetAttributeINDIRECTSPEECH_BEGINEND_4Subsents( directAndIndirectSpeechGlobalNumber );
            elements1.SetAttributeINDIRECTSPEECH_BEGINEND( directAndIndirectSpeechGlobalNumber );

            //mark begin-end IndirectSpeech-subsents-3
            subsents3.SetAttributeINDIRECTSPEECH_BEGINEND_4Subsents( directAndIndirectSpeechGlobalNumber );

            //add 2 result
            return (new SubjectObjectsTuple( subjects, objects, Id ));
        #endregion
        }
    }
    //===================================================//

    internal class IS_ThreeParentChild_01 : IS_ThreeParentChildBase
    {
        public IS_ThreeParentChild_01() : base()
        {
        }

        protected override bool IsParentMatchCondition( SubsentTuple tuple )
        {
            return (inderectspeech2.IsConditionPrecede( tuple.CHILD ) &&
                    inderectspeech2.IsCondition( tuple.CHILD ));

            //return (inderectspeech2.IsCondition( tuple.PARENT ));
        }
        protected override bool IsChildMatchCondition( SubsentTuple tuple, Language language, out IEnumerable< SubjectData > subjectData )
        {
            subjectData = null;

            var subsent    = tuple.CHILD;
            var resultList = default(List< SubjectData >);

            var sd = pattern1.GetSubjectData( subsent, language );

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
    internal class IS_ThreeParentChildGroup : IS_GroupBase< IS_ThreeParentChildBase, IS_ThreeParentChildBase.SubsentTuple >
    {
        public static IS_ThreeParentChildBase[] Get()
        {
            return
            (
                new IS_ThreeParentChildBase[]
                { 
                    new IS_ThreeParentChild_01(),
                }
            );
        }

        public IS_ThreeParentChildGroup() : base( Get() )
        {
        }

        protected override IS_ThreeParentChildBase.SubsentTuple[] GetEssence4Processing(XElement sent)
        {
            return (IS_ThreeParentChildBase.GetSubsentTuples( sent ).ToArray());
        }
        protected override SubjectObjectsTuple ProcessEssence(IS_ThreeParentChildBase pattern, IS_ThreeParentChildBase.SubsentTuple essence, Language language
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
