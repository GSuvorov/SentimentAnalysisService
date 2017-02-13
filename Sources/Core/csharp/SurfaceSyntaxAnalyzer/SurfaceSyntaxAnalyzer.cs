using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Linguistics.Core;

namespace SurfaceSyntaxAnalyzer
{
	/// <summary>
	/// Класс поверхностного синтаксического разбора
	/// </summary>
	public class SurfaceSyntaxAnalyzer
	{
		/// <summary>
		/// анализатор предложения
		/// </summary>
		private SyntaxAnalyzerBase _syntaxAnalyzer;

		#region [.ctor().]
		public SurfaceSyntaxAnalyzer()
		{
			_syntaxAnalyzer = new SyntaxAnalyzer();
		}
		#endregion

		/// <summary>
		/// Проведение поверхностного синтаксического разбора текста
		/// </summary>
		/// <param name="text">текст для разбора</param>
		public void Analyze(IText text)
		{
			foreach (var sentence in text.Sentences)
			{
				TextSentence textSentence = new TextSentence(sentence.SubsentsHierarchical);
				textSentence.SubSentences = AnalyzeSubSentenceCollection(textSentence.SubSentences);
				sentence.SetSubSentencesHeirarchy(textSentence.SubSentences.ToArray());
			}
		}

		/// <summary>
		/// Проведение поверхностного синтаксического разбора заданной коллекции подпредложений
		/// </summary>
		/// <param name="subSentences">коллекция подпредложений</param>
		/// <returns>результирующая коллекция подпредложений</returns>
		private List<SubSentence> AnalyzeSubSentenceCollection(IEnumerable<SubSentence> subSentences)
		{
			List<SubSentence> result = new List<SubSentence>();
			foreach (var subSentence in subSentences)
				result.AddRange(AnalyzeSubSentence(subSentence));
			return result;
		}

		/// <summary>
		/// Проведение поверхностного синтаксического разбора подпредложения
		/// </summary>
		/// <param name="subSentence">подпредложение</param>
		/// <returns>список подпредложений, образованных из заданного</returns>
		private SubSentence[] AnalyzeSubSentence(SubSentence subSentence)
		{
			if (subSentence.SubTextInfo != null)
			{
				Analyze(subSentence.SubTextInfo);
				return new SubSentence[] { subSentence };
			}
			else
				return AnalyzeCurrentSubSentence(subSentence);
		}

		/// <summary>
		/// Проведение поверхностного синтаксического разбора подпредложения
		/// </summary>
		/// <param name="subSentence">подпредложение</param>
		/// <returns>список подпредложений, образованных из заданного</returns>
		private SubSentence[] AnalyzeCurrentSubSentence(SubSentence subSentence)
		{
			subSentence.AppendSubSentences(AnalyzeSubSentenceCollection(subSentence.Children));
            return _syntaxAnalyzer.AnalyzeSubSentence(subSentence);
		}
	}
}
