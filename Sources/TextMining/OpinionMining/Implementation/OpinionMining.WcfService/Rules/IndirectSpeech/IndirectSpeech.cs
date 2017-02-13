using System;
using System.Collections.Generic;
using System.Linq;
using System.Xml;
using System.Xml.Linq;

using TextMining.Core;

namespace OpinionMining
{
    internal class IndirectSpeech
    {
        public IndirectSpeech( Language language )
        {
            Language = language;

            OneSubsent         = new IS_OneSubsentGroup();
            TwoSubsent         = new IS_TwoSubsentGroup();
            ThreeSubsent       = new IS_ThreeSubsentGroup();
            ParentChild        = new IS_ParentChildGroup();
            ParentChild_Desc   = new IS_ParentChild_DescGroup();
            ParentChild_DP     = new IS_ParentChild_DanglingParticipleGroup();
            ThreeParentChild   = new IS_ThreeParentChildGroup();
        }

        private Language Language
        {
            get;
            set;
        }

        private IS_OneSubsentGroup       OneSubsent
        {
            get;
            set;
        }
        private IS_TwoSubsentGroup       TwoSubsent
        {
            get;
            set;
        }
        private IS_ThreeSubsentGroup     ThreeSubsent
        {
            get;
            set;
        }
        private IS_ParentChildGroup      ParentChild
        {
            get;
            set;
        }
        private IS_ParentChild_DescGroup ParentChild_Desc
        {
            get;
            set;
        }
        private IS_ParentChild_DanglingParticipleGroup   ParentChild_DP
        {
            get;
            set;
        }
        private IS_ThreeParentChildGroup ThreeParentChild
        {
            get;
            set;
        }        

        public IList< SubjectObjectsTuple > Process( XElement sent, ref int directAndIndirectSpeechGlobalNumber, ObjectAllocateMethod objectAllocateMethod )
        {
            sent.ThrowIfNotSent();

            var opinionMiningTuples = new List< SubjectObjectsTuple >();

                ThreeSubsent.Process( sent, Language, ref directAndIndirectSpeechGlobalNumber, objectAllocateMethod )
                            .AddRangeTo( opinionMiningTuples );
                            if ( opinionMiningTuples.AnyEx() ) return (opinionMiningTuples);

                ThreeParentChild.Process( sent, Language, ref directAndIndirectSpeechGlobalNumber, objectAllocateMethod )
                                .AddRangeTo( opinionMiningTuples );
                            if ( opinionMiningTuples.AnyEx() ) return (opinionMiningTuples);

                ParentChild_DP.Process( sent, Language, ref directAndIndirectSpeechGlobalNumber, objectAllocateMethod )
                              .AddRangeTo( opinionMiningTuples );
                            if ( opinionMiningTuples.AnyEx() ) return (opinionMiningTuples);

                ParentChild.Process( sent, Language, ref directAndIndirectSpeechGlobalNumber, objectAllocateMethod )
                           .AddRangeTo( opinionMiningTuples );
                            if ( opinionMiningTuples.AnyEx() ) return (opinionMiningTuples);

                ParentChild_Desc.Process( sent, Language, ref directAndIndirectSpeechGlobalNumber, objectAllocateMethod )
                                .AddRangeTo( opinionMiningTuples );
                            if ( opinionMiningTuples.AnyEx() ) return (opinionMiningTuples);

                TwoSubsent.Process( sent, Language, ref directAndIndirectSpeechGlobalNumber, objectAllocateMethod )
                          .AddRangeTo( opinionMiningTuples );
                            if ( opinionMiningTuples.AnyEx() ) return (opinionMiningTuples);

                OneSubsent.Process( sent, Language, ref directAndIndirectSpeechGlobalNumber, objectAllocateMethod )
                          .AddRangeTo( opinionMiningTuples );
                            if ( opinionMiningTuples.AnyEx() ) return (opinionMiningTuples);

            return (opinionMiningTuples);
        }
    }
}
