using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Linguistics.Core.ContentSelectors
{
	/// <summary>
	/// Класс для выделения текста в скобках
	/// </summary>
	internal class BracketContentSelector : ContentSelectorBase
	{
		/// <summary>
		/// Положение скобки
		/// </summary>
		private struct BracketPosition
		{
			/// <summary>
			/// Скобка
			/// </summary>
			public Bracket Bracket { get; set; }
			/// <summary>
			/// Позиция скобки
			/// </summary>
			public int Position { get; set; }
		}

		#region [.ctor().]
		public BracketContentSelector()
		{
		}
		#endregion

		#region [ContentSelector]
		protected override void Select(ref string text)
		{
			var bracketList = GetBrackets(text);
			int closeBracketIndex = -1;
			for (int i = bracketList.Count - 1; i >= 0; --i)
			{
                if ( !bracketList[ i ].Bracket.IsOpen )
                {
                    closeBracketIndex = i;
                }
                else if ( (closeBracketIndex >= 0) &&
                          (bracketList[ closeBracketIndex ].Bracket.Type == bracketList[ i ].Bracket.Type)
                        )
                {
                    CreateBracketContent( ref text, bracketList[ i ], bracketList[ closeBracketIndex ] );
                    closeBracketIndex = RemoveFinishedBrackets( bracketList, i, closeBracketIndex );
                    closeBracketIndex = FindClosedBracketFromCurrent( bracketList, closeBracketIndex );
                }
			}
		}
		#endregion

		/// <summary>
		/// Поиск закрывающей скобки справа, начиная с текущей
		/// </summary>
		/// <param name="bracketList">коллекция скобок</param>
		/// <param name="currentIndex">индекс текущей скобки</param>
		/// <returns>индекс закрывающей скобки или -1, если она не найдена</returns>
		private int FindClosedBracketFromCurrent(IList<BracketPosition> bracketList, int currentIndex)
		{
			int result = currentIndex;
			while (result < bracketList.Count)
			{
				if (!bracketList[result].Bracket.IsOpen)
					return result;
				++result;
			}
			return -1;
		}

		/// <summary>
		/// Удаление обработанных скобок
		/// </summary>
		/// <param name="bracketList">коллекция скобок</param>
		/// <param name="openBracketIndex">индекс открывающей скобки</param>
		/// <param name="closeBracketIndex">индекс закрывающей скобки</param>
		/// <returns>индекс скобки, следующей за закрывающей</returns>
		private int RemoveFinishedBrackets(IList<BracketPosition> bracketList, int openBracketIndex, int closeBracketIndex)
		{
			while (closeBracketIndex > openBracketIndex + 1)
			{
				bracketList.RemoveAt(closeBracketIndex - 1);
				--closeBracketIndex;
			}
			bracketList.RemoveAt(closeBracketIndex);
			bracketList.RemoveAt(openBracketIndex);
			--closeBracketIndex;
			return closeBracketIndex;
		}

		/// <summary>
		/// Создание контента
		/// </summary>
		/// <param name="text">текст</param>
		/// <param name="startBracket">открывающая скобка</param>
		/// <param name="endBracket">закрывающая скобка</param>
		private void CreateBracketContent(ref string text, BracketPosition startBracket, BracketPosition endBracket)
		{
			int shift = _saver.GetRealPositionShift(startBracket.Position);
			int endShift = _saver.GetRealPositionShift(endBracket.Position);
			int contentStart = startBracket.Position - shift;
			int contentLength = endBracket.Position - endShift - startBracket.Position + 1;
			string content = text.Substring(contentStart, contentLength);
			if (IsBracketContent(content))
			{
				_saver.Save(new BracketContent(content, _saver.GetRealPosition(startBracket.Position),
					text.CharOrDefault(contentStart - 1),
					text.CharOrDefault(contentStart + contentLength)),
					startBracket.Position);
				text = text.Remove(contentStart, contentLength);
			}
		}

		/// <summary>
		/// Проверка, что заданный текст необходимо анализировать отдельно
		/// </summary>
		/// <param name="text">текст</param>
		/// <returns>результат проверки</returns>
		private bool IsBracketContent(string text)
		{
			bool isFewLetters = text.Count(_ => Char.IsLetter(_)) > 1;
			bool isContainLineBrackets = text.Any(_ => (_ == '\r') || (_ == '\n'));
			return isFewLetters && !isContainLineBrackets;
		}

		/// <summary>
		/// Получение списка скобок в тексте
		/// </summary>
		/// <param name="text">текст</param>
		/// <returns>список пар "скобка-позиция"</returns>
		private List<BracketPosition> GetBrackets(string text)
		{
			List<BracketPosition> result = new List<BracketPosition>();
			for (int i = 0; i < text.Length; ++i)
			{
				Bracket bracket = Bracket.Create(text[i]);
				if (bracket != null)
					result.Add(new BracketPosition() { Bracket = bracket, Position = i });
			}
			return result;
		}
	}
}
