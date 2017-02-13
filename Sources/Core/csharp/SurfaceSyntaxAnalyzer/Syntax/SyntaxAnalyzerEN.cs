using Linguistics.Core;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace SurfaceSyntaxAnalyzer
{
    /// <summary>
    /// Класс анализатора предложения на английском языке
    /// </summary>
    internal class SyntaxAnalyzerEN : SyntaxAnalyzerBase
    {
        #region [SentenceAnalyzerBase]
        public override SubSentence[] AnalyzeSubSentence(SubSentence subSentence)
        {
            return new SubSentence[] { subSentence };
        }
        #endregion
    }
}
