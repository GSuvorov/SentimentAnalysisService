using Linguistics.Core;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace SurfaceSyntaxAnalyzer
{
    /// <summary>
    /// Класс анализатора предложения
    /// </summary>
    internal class SyntaxAnalyzer : SyntaxAnalyzerBase
    {
        /// <summary>
        /// анализаторы для разных языков
        /// </summary>
        private Dictionary<string, SyntaxAnalyzerBase> _syntaxAnalyzers;

        #region [.ctor().]
        public SyntaxAnalyzer()
        {
            _syntaxAnalyzers = new Dictionary<string, SyntaxAnalyzerBase>();
            #region [.Russian syntax initialization.]
            SyntaxAnalyzerBase syntaxAnalyzerRU = new SyntaxAnalyzerRU();

            /// декораторы, которые работают перед заданным компонентом
            syntaxAnalyzerRU = new QuestionWordDecorator(syntaxAnalyzerRU);
            syntaxAnalyzerRU = new HomogeneousDecorator(syntaxAnalyzerRU);
            syntaxAnalyzerRU = new CompoundVerbDecorator(syntaxAnalyzerRU);
            syntaxAnalyzerRU = new ConjunctionDecorator(syntaxAnalyzerRU);
            syntaxAnalyzerRU = new IntroductoryDecorator(syntaxAnalyzerRU);
            syntaxAnalyzerRU = new SeparatorDecorator(syntaxAnalyzerRU);

            /// декораторы, которые работают после заданного компонента
            syntaxAnalyzerRU = new UnmarkedUnionDecorator(syntaxAnalyzerRU);
            syntaxAnalyzerRU = new RoleDecorator(syntaxAnalyzerRU);
            syntaxAnalyzerRU = new PassiveSubjectDecorator(syntaxAnalyzerRU);
            _syntaxAnalyzers.Add(Language.Russian, syntaxAnalyzerRU);
            #endregion
            #region [.English syntax initialization.]
            SyntaxAnalyzerBase syntaxAnalyzerEN = new SyntaxAnalyzerEN();

            /// декораторы, которые работают после заданного компонента
            syntaxAnalyzerEN = new UnmarkedUnionDecorator(syntaxAnalyzerEN);
            syntaxAnalyzerEN = new RoleDecorator(syntaxAnalyzerEN);
            _syntaxAnalyzers.Add(Language.English, syntaxAnalyzerEN);
            #endregion
        }
        #endregion

        #region [SyntaxAnalyzerBase]
        public override SubSentence[] AnalyzeSubSentence(SubSentence subSentence)
        {
            SyntaxAnalyzerBase analyzer;
            if ((subSentence.ParentObject.Language != null) &&
                _syntaxAnalyzers.TryGetValue(subSentence.ParentObject.Language, out analyzer))
            {
                return analyzer.AnalyzeSubSentence(subSentence);
            }
            else
                return new SubSentence[] { subSentence };
        }
        #endregion
    }
}
