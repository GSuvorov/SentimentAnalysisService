using System.Collections.Generic;
using System.Linq;
using System.Xml.Linq;

using TextMining.Core;

namespace OpinionMining
{
    internal class Rules
    {
        public Rules( Language language )
        {
            Language = language;

            IndirectSpeech = new IndirectSpeech( language );
            DirectSpeech   = new DirectSpeech  ( language );
        }

        public Language Language
        {
            get;
            private set;
        }

        public IndirectSpeech IndirectSpeech
        {
            get;
            private set;
        }
        public DirectSpeech   DirectSpeech
        {
            get;
            private set;
        }

        public IEnumerable< SubjectObjectsTuple > Process( XElement sent, ref int directAndIndirectSpeechGlobalNumber, ObjectAllocateMethod objectAllocateMethod )
        {
            var result = DirectSpeech.Process( sent, ref directAndIndirectSpeechGlobalNumber, objectAllocateMethod );

            if ( !result.AnyEx() )
            {
                result = IndirectSpeech.Process( sent, ref directAndIndirectSpeechGlobalNumber, objectAllocateMethod  );
            }
            else if ( result.Count == 1 )
            {
                /*
                Если DirectSpeech отработало (первое условие), то смотрим только один шаблон: ParentChild-DanglingParticiple, 
                с условием, что субъект высказывания ENTITY SUBJECT="TRUE" остался тот  же (второе условие). 
                */
                var sot = IS_ParentChild_DanglingParticipleAfterDSGroup.Instance.Process
                                 (
                                 sent,
                                 Language,
                                 directAndIndirectSpeechGlobalNumber,
                                 objectAllocateMethod,
                                 result.First()
                                 );
                if ( sot != null )
                {
                    return (sot.RepeatOnce());
                }
            }

            return (result);
        }
    }
    //===================================================//
}



