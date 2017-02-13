#if Create_NodeOffsetMap_Over_Mikl_Algorithm
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace OffsetsBases
{
	internal class TextTerm
	{
		public TextTerm( string name, int offset, int index )
		{
			this.Name   = name;
			this.Offset = offset;
			this.Index  = index;
		}

		public int    Offset
        {
            get;    
            private set;
        }
		public int    Index
        {
            get;    
            private set;
        }
        public string Name
        {
            get;
            private set;
        }
	}

	public class OffsetGenerator
	{
		private static readonly Dictionary<char, char> _similarCharMap = new Dictionary<char, char>()
		{
			{'A', 'А'}, {'a', 'а'}, {'B', 'В'}, {'C', 'С'}, {'c', 'с'}, {'E', 'Е'}, {'e', 'е'}, 
			{'H', 'Н'}, {'K', 'К'}, {'k', 'к'}, {'M', 'М'}, {'o', 'о'}, {'O', 'О'}, {'P', 'Р'}, 
			{'p', 'р'}, {'T', 'Т'}, {'X', 'Х'}, {'x', 'х'}, {'Y', 'У'}, {'y', 'у'}
		};

		private TextTerm[] _termList;
		private Dictionary<string, List<TextTerm>> _termListIndex;

		private StringBuilder _lastErrors = new StringBuilder();

		//Максимально допустимое число ошибок в процентах от размера ТV {0-99}
		public int MaxErrorPersent = 0;

		public string LastError
		{
			get { return _lastErrors.ToString(); }
		}

		public OffsetGenerator(string text)
		{
			createTextTV(text);
		}

		public static string[] Tokenize(string text)
		{
			bool digitTerm = false;
			var termList = new List<string>();
			StringBuilder termName = new StringBuilder();
			for (int i = 0; i < text.Length; i++)
			{
				char ch = text[i];

				if (Char.IsLetter(ch))
				{
					if (termName.Length > 0 && digitTerm)
					{
						termList.Add(termName.ToString());
						termName.Clear();
						digitTerm = false;
					}
					//регистровая нормализация
					termName.Append(Char.ToLower(ch));
					continue;
				}
				if (Char.IsDigit(ch))
				{
					if (termName.Length > 0 && !digitTerm)
					{
						termList.Add(termName.ToString());
						termName.Clear();
					}
					termName.Append(ch);
					digitTerm = true;
					continue;
				}
				else
				{
					digitTerm = false;
				}
				//Добавление терма
				if (termName.Length > 0)
				{
					termList.Add(termName.ToString());
					termName.Clear();
				}
				//добавление терма - односимвольная пунктуация
				if (!isSpaceSymbol(ch))
				{
					termList.Add(ch.ToString());
				}
			}
			if (termName.Length > 0)
			{
				termList.Add(termName.ToString());
			}
			return termList.ToArray();
		}

		//Идентификация подвектора термов в полном векторе всех термов документа
		public int[] GetOffsetList(string[] termList)
		{
			_lastErrors.Clear();

			int termErrorCount = 0;
			int maxErrorCount = MaxErrorPersent * termList.Length / 100;

			//Терм из искомого вектора с минимальным числом вхождений в документ
			int minTermIndex = -1;
			List<TextTerm> minTerm = null;
			//список списков термов документа, входящих в искомый вектор
			var textTermList = termList.Select((s, idx) =>
				{
					List<TextTerm> tt;
					if (!_termListIndex.TryGetValue(s, out tt))
					{
						_lastErrors.AppendFormat("Vector term not found: [{0}]\n", s);
						termErrorCount++;
						if (termErrorCount > maxErrorCount)
						{
							throw new ApplicationException(_lastErrors.ToString());
						}
						return null;
					}
					if (minTerm == null || minTerm.Count > tt.Count)
					{
						minTerm = tt;
						minTermIndex = idx;
					}
					return tt;
				}
			).ToArray();

			int maxDistance = termList.Length / 2;
			TextTerm[] bestRCTermList = null;
			int bestSumDifference = 0;
			foreach (var textTerm in minTerm)
			{
				int sumDifference = 0;
				int startTermIndex = textTerm.Index - minTermIndex;

				var rcTermList = textTermList.Select((t, idx) =>
					{
						if (t == null) return null;
						//Поиск ближайшего совпадения терма
						var tl = t.Select(x => new { Term = x, Distance = Math.Abs(x.Index - startTermIndex - idx) }).
							Where(x => x.Distance <= maxDistance).OrderBy(x => x.Distance);
						var term = tl.Select(x => x.Term).FirstOrDefault();
						if (term == null)
						{
							sumDifference += maxDistance;
							startTermIndex--;
							return null;
						}
						if (term.Index != startTermIndex + idx)
						{
							sumDifference += Math.Abs(term.Index - startTermIndex - idx);
							startTermIndex += term.Index - startTermIndex - idx;
						}
						return term;
					}
				).ToArray();

				if (bestRCTermList == null || bestSumDifference > sumDifference)
				{
					bestSumDifference = sumDifference;
					bestRCTermList = rcTermList;
				}
			}
			//логирование степени несовпадения
			if (bestSumDifference > 0)
			{
				_lastErrors.AppendFormat("Difference={0}", bestSumDifference);
			}
			//возвращение результата
			int prevOffset = 0;
			return bestRCTermList.Select(t =>
				{
					if (t != null) prevOffset = t.Offset;
					return prevOffset;
				}
			).ToArray();
		}

		//Создание терм-вектора для текста
		private void createTextTV(string text)
		{
			_termListIndex = new Dictionary<string, List<TextTerm>>();
			var termList = new List<TextTerm>();

			//разбиение текста на термы
			bool digitTerm = false;
			StringBuilder termName = new StringBuilder();
			int offset = 0;
			char prev = (char)0;
			for (int i = 0; i < text.Length; i++)
			{
				char ch = text[i];

				if (Char.IsLetter(ch))
				{
					if (digitTerm && termName.Length > 0)
					{
						addTerm(termName.ToString(), offset, termList);
						termName.Clear();
						digitTerm = false;
					}
					//замена английских символов, схожих по написанию на русские в русских словах
					if (isEngSymbol(ch))
					{
						char ruChar;
						if (_similarCharMap.TryGetValue(ch, out ruChar))
						{
							//проверка на соседние русские символы
							if (isRusSymbol(prev) ||
								!Char.IsLetter(prev) && i < text.Length - 1 && isRusSymbol(Char.ToLower(text[i + 1])))
							{
								ch = ruChar;
							}
						}
					}
					//регистровая нормализация
					ch = Char.ToLower(ch);
					//замена буквы ё на е
					if (ch == 0x0451)
					{
						ch = (char)0x0435;
					}
					if (termName.Length == 0)
					{
						offset = i;
					}
					termName.Append(ch);
					prev = ch;
					continue;
				}
				if (Char.IsDigit(ch))
				{
					if (!digitTerm && termName.Length > 0)
					{
						addTerm(termName.ToString(), offset, termList);
						termName.Clear();
					}
					if (termName.Length == 0)
					{
						offset = i;
						digitTerm = true;
					}
					termName.Append(ch);
					prev = ch;
					continue;
				}
				else
				{
					digitTerm = false;
				}
				prev = ch;
				//Добавление терма
				if (termName.Length > 0)
				{
					addTerm(termName.ToString(), offset, termList);
					termName.Clear();
				}
				//добавление терма - односимвольная пунктуация
				if (!isSpaceSymbol(ch))
				{
					addTerm(ch.ToString(), i, termList);
				}
			}
			if (termName.Length > 0)
			{
				addTerm(termName.ToString(), offset, termList);
			}

			_termList = termList.ToArray();
		}

		private void addTerm(string name, int offset, List<TextTerm> termList)
		{
			var term = new TextTerm(name.ToString(), offset, termList.Count);
			termList.Add(term);
			List<TextTerm> terms;
			if (!_termListIndex.TryGetValue(name, out terms))
			{
				terms = new List<TextTerm>();
				_termListIndex.Add(name, terms);
			}
			terms.Add(term);

		}

		private static bool isRusSymbol(char ch)
		{
			return (ch >= 0x0410 && ch <= 0x042F || ch >= 0x0430 && ch <= 0x044F) || ch == 0451 || ch == 0401;
		}
		private static bool isEngSymbol(char ch)
		{
			return (ch >= 0x0041 && ch <= 0x005A || ch >= 0x0061 && ch <= 0x007A);
		}
		private static bool isSpaceSymbol(char ch)
		{
			return (ch < 0x0020L) ||
				(ch > 0xD7FFL && ch < 0xE000) ||
				(ch > 0xFFFDL) ||
				Char.IsWhiteSpace(ch);
		}
	}
}
#endif