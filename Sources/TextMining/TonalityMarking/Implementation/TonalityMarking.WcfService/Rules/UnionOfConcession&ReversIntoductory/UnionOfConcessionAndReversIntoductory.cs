using System;
using System.Collections.Generic;
using System.Linq;
using System.Xml.Linq;
using System.Xml.XPath;
using System.Xml;

using TextMining.Core;

namespace TonalityMarking
{
    /// <summary>
    /// UnionOfConcession & ReversIntoductory
    /// </summary>
    internal class UnionOfConcessionAndReversIntoductory
    {
        public UnionOfConcessionAndReversIntoductory( XElement rootRulesNode )
        {
            var unionOfConcessionNode = rootRulesNode.GetSingleNode( "UnionOfConcession" );
            UnionOfConcession = new UnionOfConcession( unionOfConcessionNode );

            var reversIntoductoryNode = rootRulesNode.GetSingleNode( "ReversIntoductory" );
            ReversIntoductory = new ReversIntoductory( reversIntoductoryNode );

            _RulesByOrder = UnionOfConcession.RulesByOrder
                           .Concat
                           (
                           ReversIntoductory.RulesByOrder
                           )
                           .ToArray();
        }

        public UnionOfConcession UnionOfConcession
        {
            get;
            private set;
        }
        public ReversIntoductory ReversIntoductory
        {
            get;
            private set;
        }

        private RuleBase[] _RulesByOrder;
        public IEnumerable< RuleBase > RulesByOrder
        {
            get { return (_RulesByOrder); }
        }
    }
}



