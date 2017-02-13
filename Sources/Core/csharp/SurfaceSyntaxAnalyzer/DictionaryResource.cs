using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Linguistics.Core;
using System.Text.RegularExpressions;
using Utils;
using System.IO;

namespace SurfaceSyntaxAnalyzer
{
	/// <summary>
	/// Класс для различных словарей, необходимых для синтаксического разбора
	/// </summary>
	internal static class DictionaryResource
	{
		#region [.dictionaries.]
		/// <summary>
		/// Частицы отрицания
		/// </summary>
		private static readonly string[] _negotiationParticles = new string[] { "ни", "не" };

		/// <summary>
		/// Прочие перечисления
		/// </summary>
		private static readonly string[] _otherEnumeration = new string[]
			{ "др.", "др", "тд.", "тд", "т.д.", "пр.", "пр", "edc.", "edc", "тп.", "тп", "т.п." };

		/// <summary>
		/// Однородные союзы
		/// </summary>
		private static readonly string[] _homogeneousConjunctions = new string[] { "и", "или", "да", "либо" };

		/// <summary>
		/// словари союзов и союзных слов, перед которыми обязательная запятая
		/// </summary>
		private static Dictionary<SubSentenceType, string[]> _conjunctionDict;

		/// <summary>
		/// Коллекция исключений из союзов и союзных слов
		/// </summary>
		private static readonly string[] _conjunctionExclusions = new string[] { "как раз и", "как раз" };

		/// <summary>
		/// вводные слова и словосочетания
		/// </summary>
		private static readonly HashSet<string> _parenthesis;

		/// <summary>
		/// обстоятельственные слова
		/// </summary>
        private static readonly HashSet<string> _adjuncts;

		/// <summary>
		/// неопределенные местоимения
		/// </summary>
        private static readonly HashSet<string> _indefinitePronouns;
		#endregion

		#region [.selectors.]
		/// <summary>
		/// Регулярное выражение для выделения союзов и союзных слов
		/// </summary>
		private static readonly Regex _conjunctionRegex;
		/// <summary>
		/// Регулярное выражение для выделения исключений из союзов и союзных слов
		/// </summary>
		private static readonly Regex _conjunctionExclusionsRegex;
		/// <summary>
		/// Регулярное выражение для выделения отдельных текстовых элементов
		/// </summary>
		private static readonly Regex _individualTextItemRegex;
		/// <summary>
		/// Регулярное выражение для выделение прочих перечислений
		/// </summary>
		private static readonly Regex _otherEnumerationRegex;
		/// <summary>
		/// Выделитель союзов
		/// </summary>
		private static ConjunctionSelector _selector = new ConjunctionSelector();
		#endregion

		#region [.ctor().]
		static DictionaryResource()
		{
			_conjunctionDict = new Dictionary<SubSentenceType,string[]>();
			_conjunctionDict.Add(SubSentenceType.Subordinate, new string[]
				{ "кто", "о том что", "потому что", "притом что", "оттого что", "так что", "что", "какой", "какого",
				  "какому", "каким", "каком", "какая", "какую", "какое", "какие", "каких", "какими", "чьей", "чей", "чья",
				  "чьи", "чье", "чьё", "которой", "который", "которая", "которые", "которое", "которому", "которым",
				  "которого", "которых", "которую", "которою", "которыми", "котором", "где", "куда", "когда", "откуда",
				  "как будто", "будто бы", "как", "будто", "словно", "насколько", "сколько", "сколь", "насколь", "зачем",
				  "почему", "отчего", "чтоб", "чтобы", "как бы", "пока", "едва", "прежде чем", "так как", "т.к.", "т. к.",
				  "ибо", "благо", "поскольку", "дабы", "если", "раз", "ежели", "коль скоро", "пусть", "пускай", "кому", "чему" });
			_conjunctionDict.Add(SubSentenceType.Default, new string[]
				{ "причем", "то есть", "т.е.", "т. е.", "а именно", "а также", "но", "а", "зато", "однако" });

			_conjunctionRegex = CreateConjunctionRegex();
			_conjunctionExclusionsRegex = new Regex(Utils.RegexHelper.GetVariantsFromCollection(_conjunctionExclusions));
			_otherEnumerationRegex = new Regex("^" + Utils.RegexHelper.GetVariantsFromCollection(_otherEnumeration) + "$");
			_individualTextItemRegex = CreateIndividualTextItemRegex();

			string path = Path.Combine(Utils.GlobalVariables.UserResourcesPath, @"SentenceSeparation");
			_parenthesis = Utils.Dictionary.ReadAsHashSet(Path.Combine(path, "parenthesis.txt"), Encoding.Unicode);
			_adjuncts = Utils.Dictionary.ReadAsHashSet(Path.Combine(path, "adjuncts.txt"), Encoding.Unicode);
			_indefinitePronouns = Utils.Dictionary.ReadAsHashSet(Path.Combine(path, "indefinite pronouns.txt"), Encoding.Unicode);
		}

		/// <summary>
		/// Создание регулярного выражения для выделения союзов и союзных слов
		/// </summary>
		/// <returns>регулярное выражение</returns>
		private static Regex CreateConjunctionRegex()
		{
			List<string> conjunctions = new List<string>();
			foreach (var dictionary in _conjunctionDict)
				conjunctions.AddRange(dictionary.Value);
			conjunctions.AddRange(_homogeneousConjunctions);
			return new Regex(Utils.RegexHelper.GetVariantsFromCollection(conjunctions));
		}

		/// <summary>
		/// Создание регулярного выражения для выделения отдельных текстовых элементов
		/// </summary>
		/// <returns>регулярное выражение</returns>
		private static Regex CreateIndividualTextItemRegex()
		{
			var collection = _otherEnumeration.Union(_negotiationParticles);
			return new Regex(Utils.RegexHelper.GetVariantsFromCollection(collection));
		}
		#endregion

		#region [.public.]
		/// <summary>
		/// Проверка, что заданный текст является союзом заданного типа
		/// </summary>
		/// <param name="conjunction">текст</param>
		/// <param name="type">тип союза</param>
		/// <returns>результат проверки</returns>
		public static bool IsConjunctionType(string text, SubSentenceType type)
		{
			return IsConjunctionCurrentType(new SimpleStringBuilder(text).LowerValue, type);
		}

		/// <summary>
		/// Проверка, что заданный текст является союзом однородных
		/// </summary>
		/// <param name="text">текст</param>
		/// <returns>результат проверки</returns>
		public static bool IsHomogeneousConjunction(string text)
		{
			return IsHomogeneousCurrentConjunction(new SimpleStringBuilder(text).LowerValue);
		}

		/// <summary>
		/// Проверка, что заданный текст является частицей отрицания
		/// </summary>
		/// <param name="text">текст</param>
		/// <returns>результат проверки</returns>
		public static bool IsNegotiationParticle(string text)
		{
			return _negotiationParticles.Contains(new SimpleStringBuilder(text).LowerValue);
		}

		/// <summary>
		/// Проверка, что заданный текст является прочим перечислением
		/// </summary>
		/// <param name="text">текст</param>
		/// <returns>результат проверки</returns>
		public static bool IsOtherEnumeration(string text)
		{
			return _otherEnumerationRegex.IsMatch(new SimpleStringBuilder(text).LowerValue);
		}

		/// <summary>
		/// Проверка, что заданный текст является вводным словом или словосочетанием
		/// </summary>
		/// <param name="text">текст</param>
		/// <returns>результат проверки</returns>
		public static bool IsParenthesis(string text)
		{
			return _parenthesis.Contains(new SimpleStringBuilder(text).LowerValue);
		}

		/// <summary>
		/// Проверка, что заданный текст является неопределенным местоимением
		/// </summary>
		/// <param name="text">текст</param>
		/// <returns>результат проверки</returns>
		public static bool IsIndefinitePronoun(string text)
		{
			return _indefinitePronouns.Contains(new SimpleStringBuilder(text).LowerValue);
		}

		/// <summary>
		/// Проверка, что заданный текст начинается с обстоятельственного слова
		/// </summary>
		/// <param name="text">текст</param>
		/// <returns>результат проверки</returns>
		public static bool IsStartWithAdjunct(string text)
		{
			string simpleText = new SimpleStringBuilder(text).LowerValue;
			foreach (string adjunct in _adjuncts)
				if (simpleText.StartsWith(adjunct))
					return true;
			return false;
		}

		/// <summary>
		/// Выделение союзов и союзных слов из заданной коллекции юнитов
		/// </summary>
		/// <param name="units">коллекция юнитов</param>
		/// <returns>список союзов и союзных слов</returns>
		public static List<Conjunction> SelectConjunctions(IEnumerable<UnitTextBase> units)
		{
			return _selector.SelectConjunctions(units);
		}

		/// <summary>
		/// Выделение отдельных текстовых элементов из заданного юнита
		/// </summary>
		/// <param name="unit">юнит</param>
		/// <returns>список отдельных текстовых элементов</returns>
		public static List<TextPart> SelectIndividualTextItems(UnitTextBase unit)
		{
			List<TextPart> result = new List<TextPart>();
			foreach (Match match in _individualTextItemRegex.Matches(unit.Text.ToLower()))
			{
				if (IsPossibleNearSymbols(unit.Text, match))
					result.Add(new TextPart() { Start = match.Index, End = match.Index + match.Length });
			}
			return result;
		}
		#endregion

		#region [.private.]
		/// <summary>
		/// Проверка, что заданный текст является союзом заданного типа
		/// </summary>
		/// <param name="conjunction">текст</param>
		/// <param name="type">тип союза</param>
		/// <returns>результат проверки</returns>
		private static bool IsConjunctionCurrentType(string text, SubSentenceType type)
		{
			return _conjunctionDict[type].Any(_ => _ == text);
		}

		/// <summary>
		/// Проверка, что заданный текст является союзом однородных
		/// </summary>
		/// <param name="text">текст</param>
		/// <returns>результат проверки</returns>
		private static bool IsHomogeneousCurrentConjunction(string text)
		{
			return _homogeneousConjunctions.Contains(text);
		}

		/// <summary>
		/// Получение типа заданного союза
		/// </summary>
		/// <param name="conjunction">союз</param>
		/// <returns>тип союза</returns>
		private static SubSentenceType GetConjunctionType(string conjunction)
		{
			if (IsHomogeneousCurrentConjunction(conjunction))
				return SubSentenceType.Default;
			foreach (var type in _conjunctionDict.Keys)
				if (IsConjunctionType(conjunction, type))
					return type;
			throw new InvalidOperationException("Cannot find conjunction type");
		}

		/// <summary>
		/// Проверка допустимости рядом стоящих символов для заданного выделенного интервала
		/// </summary>
		/// <param name="text">текст</param>
		/// <param name="match">интервал</param>
		/// <returns>результат проверки</returns>
		private static bool IsPossibleNearSymbols(string text, Match match)
		{
			return IsPossibleNearConjunctionSymbol(text.GetPreviousSymbol(match.Index)) &&
				IsPossibleNearConjunctionSymbol(text.GetNextSymbol(match.Index + match.Length - 1));
		}

		/// <summary>
		/// Проверка, что заданный символ может стоять рядом с союзом
		/// </summary>
		/// <param name="symbol">символ</param>
		/// <returns>результат проверки</returns>
		private static bool IsPossibleNearConjunctionSymbol(char symbol)
		{
			return !(Char.IsLetterOrDigit(symbol) || (symbol == '-'));
		}

		/// <summary>
		/// Класс для выделения союзов
		/// </summary>
		private class ConjunctionSelector
		{
			/// <summary>
			/// сдвиг союзов
			/// </summary>
			private int _conjunctionShift;
			/// <summary>
			/// исходный текст
			/// </summary>
			private string _sourceText;
			/// <summary>
			/// Строитель для исходного текста
			/// </summary>
			private SimpleStringBuilder _sourceBuilder;

			/// <summary>
			/// Инициализация внутренних свойств из заданной коллекции юнитов
			/// </summary>
			/// <param name="units">коллекция юнитов</param>
			private void Initialize(IEnumerable<UnitTextBase> units)
			{
				_sourceText = ReplaceExclusions(units.GetText());
				_conjunctionShift = units.First().PositionInfo.Start;
				_sourceBuilder = new SimpleStringBuilder(_sourceText);
			}

			/// <summary>
			/// Замена исключений в заданном тексте
			/// </summary>
			/// <param name="sourceText">текст</param>
			/// <returns>результирующая строка</returns>
			private string ReplaceExclusions(string sourceText)
			{
				StringBuilder text = new StringBuilder(sourceText);
				SimpleStringBuilder builder = new SimpleStringBuilder(text.ToString());
				foreach (Match match in _conjunctionExclusionsRegex.Matches(builder.LowerValue))
				{
					if (IsPossibleNearSymbols(builder.LowerValue, match))
					{
						int start = builder.GetRealPosition(match.Index);
						int length = builder.GetRealPosition(match.Index + match.Length) - start;
						text.Replace(sourceText.Substring(start, length), new string('_', length), start, length);
					}
				}
				return text.ToString();
			}

			/// <summary>
			/// Выделение союзов и союзных слов из заданной коллекции юнитов
			/// </summary>
			/// <param name="units">коллекция юнитов</param>
			/// <returns>список союзов и союзных слов</returns>
			public List<Conjunction> SelectConjunctions(IEnumerable<UnitTextBase> units)
			{
				List<Conjunction> result = new List<Conjunction>();
				Initialize(units);
				foreach (Match match in _conjunctionRegex.Matches(_sourceBuilder.LowerValue))
				{
					if (IsConjunction(units, match))
						result.Add(CreateConjunction(match));
				}
				return result;
			}

			/// <summary>
			/// Проверка, что заданное найденное значение союза является отдельным союзом
			/// </summary>
			/// <param name="units">коллекция юнитов</param>
			/// <param name="conjunctionMatch">найденное значение союза</param>
			/// <returns>результат проверки</returns>
			private bool IsConjunction(IEnumerable<UnitTextBase> units, Match conjunctionMatch)
			{
				return IsPossibleNearSymbols(_sourceBuilder.LowerValue, conjunctionMatch) &&
					!units.Any(_ => _.IsEntity && IsConjunctionSplitEntity((Entity)_, conjunctionMatch));
			}

			/// <summary>
			/// Проверка, что заданное найденное значение союза разрезает сущность
			/// </summary>
			/// <param name="entity">сущность</param>
			/// <param name="conjunctionMatch">найденное значение союза</param>
			/// <returns>результат проверки</returns>
			private bool IsConjunctionSplitEntity(Entity entity, Match conjunctionMatch)
			{
				int realStart = _conjunctionShift + _sourceBuilder.GetRealPosition(conjunctionMatch.Index);
				int realEnd = _conjunctionShift + _sourceBuilder.GetRealPosition(conjunctionMatch.Index + conjunctionMatch.Length);
				return !IsConjunctionContainEntity(entity, realStart, realEnd) &&
					(entity.IsPositionInsideEntity(realStart) || entity.IsPositionInsideEntity(realEnd));
			}

			/// <summary>
			/// Проверка, что союз полностью содержит сущность
			/// </summary>
			/// <param name="entity">сущность</param>
			/// <param name="conjunctionStart">позиция начала союза</param>
			/// <param name="conjunctionEnd">позиция конца союза</param>
			/// <returns>результат проверки</returns>
			private bool IsConjunctionContainEntity(Entity entity, int conjunctionStart, int conjunctionEnd)
			{
				return (conjunctionStart <= entity.PositionInfo.Start) && (entity.PositionInfo.End <= conjunctionEnd);
			}

			/// <summary>
			/// Создание союза из заданного найденного значения союза
			/// </summary>
			/// <param name="conjunctionMatch">найденное значение союза</param>
			/// <returns>новый союз</returns>
			private Conjunction CreateConjunction(Match conjunctionMatch)
			{
				int realStart = _sourceBuilder.GetRealPosition(conjunctionMatch.Index);
				int realEnd = _sourceBuilder.GetRealPosition(conjunctionMatch.Index + conjunctionMatch.Length);
				return new Conjunction()
					{
						Value = _sourceText.Substring(realStart, realEnd - realStart),
						StartPosition = _conjunctionShift + realStart,
						EndPosition = _conjunctionShift + realEnd,
						Type = GetConjunctionType(conjunctionMatch.Value)
					};
			}
		}
		#endregion
	}
}
