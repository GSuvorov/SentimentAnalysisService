using System;
using System.Collections.Generic;
using System.Linq;
using System.Xml;
using System.Xml.Linq;

namespace OpinionMining
{
    using OpinionMining.DirectIndirectSpeechExtensions;
    using TextMining.Core;

    internal abstract class IS_OneSubsentBase : RuleBase
    {
        public IS_OneSubsentBase( string id = null ) : base( id )
        {
        }

        public static IEnumerable< XElement > GetSubsents( XElement sent )
        {
            sent.ThrowIfNotSent();

            var subsents   = from subsent in sent.DescendantsSubsentence()
                             where subsent.AttributeTypeAreEqual( SubsentenceType.Subordinate,
                                                                  SubsentenceType.Default, 
                                                                  SubsentenceType.Introductory )

                             where subsent.IsSubsentNotHasObjectAndSubject() //.IsSubsentNoThereWasInRules()

                             select subsent;
            return (subsents);
        }

        protected abstract bool IsSubsentMatchCondition( XElement subsent, Language language, out IEnumerable< SubjectData > subjectData, out XElement elementU );

        public SubjectObjectsTuple Process( XElement subsent, Language language
            , ref int directAndIndirectSpeechGlobalNumber, ObjectAllocateMethod objectAllocateMethod )
        {subsent.ThrowIfNotSubsent();

        #region [.condition.]
            var subjects = default(IEnumerable< SubjectData >);
            var elementU = default(XElement);
            if ( !IsSubsentMatchCondition( subsent, language, out subjects, out elementU ) )
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
            var objects = elementU.ElementsAfterSelfExceptT().TryAllocateObjects( objectAllocateMethod, id );

            //mark begin-end IndirectSpeech
            elementU.ElementsAfterSelfExceptT().SetAttributeINDIRECTSPEECH_BEGINEND( directAndIndirectSpeechGlobalNumber );

            //add 2 result
            return (new SubjectObjectsTuple( subjects, objects, Id ) );
        #endregion
        }
    }
    //===================================================//

    /*
      <pattern id="IS_OneSubsent_01">
        <examples>
          <example>Как заявил Петя - мы лучше всех.</example>
          <example>Как дал понять Петя - сегодня пить пиво.</example>
        </examples>
        <!--"IS_OneSubsent_02"-->
        <examples>
          <example>Петя дал всем понять  - сегодня пить пиво.</example>
        </examples>
        
        <SUB_SENT TYPE="Subordinate || Default || Introductory">
          [::pattern1::] + [::pattern6::]
          <U> - </U>
          [::inderectspeech6::]
        </SUB_SENT>
      </pattern>
     */
    internal class IS_OneSubsent_01 : IS_OneSubsentBase
    {
        public IS_OneSubsent_01(): base()
        {
        }

        private static XElement GetElementU( XElement startElement )
        {
            return (startElement.ElementsAfterSelfExceptT().GetElementU( separator1.uConditionFunction ));
        }

        protected override bool IsSubsentMatchCondition( XElement subsent, Language language, out IEnumerable< SubjectData > subjectData, out XElement elementU )
        {
            elementU    = null;
            subjectData = null;

            var resultList = default(List< SubjectData >);
            var sd         = default(SubjectData);

            sd = pattern1 .GetSubjectData( subsent, language ); if ( sd.IsNotNull() ) goto NEXT;
            sd = pattern6 .GetSubjectData( subsent, language ); if ( sd.IsNotNull() ) goto NEXT;
            sd = pattern18.GetSubjectData( subsent, language ); if ( sd.IsNotNull() ) goto NEXT;
            sd = pattern19.GetSubjectData( subsent, language ); if ( sd.IsNotNull() ) goto NEXT;
            sd = pattern20.GetSubjectData( subsent, language ); if ( sd.IsNotNull() ) goto NEXT;
            sd = pattern21.GetSubjectData( subsent, language ); if ( sd.IsNotNull() ) goto NEXT;
            sd = pattern22.GetSubjectData( subsent, language ); if ( sd.IsNotNull() ) goto NEXT;
            sd = pattern23.GetSubjectData( subsent, language ); if ( sd.IsNotNull() ) goto NEXT;
            sd = pattern24.GetSubjectData( subsent, language ); if ( sd.IsNotNull() ) goto NEXT;

        NEXT:
            sd.Add2List( ref resultList );

            if ( resultList.AnyEx() )
            {
                resultList = resultList.MakeDistinct();

                var rightExtremeElement = resultList.GetRightExtremeElement();
                if ( (elementU = GetElementU( rightExtremeElement )) == null )
                    return (false);

                subjectData = resultList;
                return (true);
            }

            return (false);
        }
    }
    //===================================================//

    internal class IS_OneSubsentGroup : IS_GroupBase< IS_OneSubsentBase, XElement >
    {
        private static IS_OneSubsentBase[] Get()
        {
            return
            (
                new IS_OneSubsentBase[]
                { 
                    new IS_OneSubsent_01(),
                }
            );
        }

        public IS_OneSubsentGroup() : base( Get() )
        {
        }

        protected override XElement[] GetEssence4Processing( XElement sent )
        {
            return (IS_OneSubsentBase.GetSubsents( sent ).ToArray());
        }
        protected override SubjectObjectsTuple ProcessEssence( IS_OneSubsentBase pattern, XElement essence, Language language
            , ref int directAndIndirectSpeechGlobalNumber, ObjectAllocateMethod objectAllocateMethod )
        {
            if ( essence.IsSubsentNotHasObjectAndSubject() )
            {
                return (pattern.Process( essence, language, ref directAndIndirectSpeechGlobalNumber, objectAllocateMethod ));
            }
            return (null);
        }
    }
}
