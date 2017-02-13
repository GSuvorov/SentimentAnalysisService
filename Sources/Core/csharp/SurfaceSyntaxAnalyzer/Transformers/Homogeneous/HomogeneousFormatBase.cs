using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Text.RegularExpressions;
using Linguistics.Core;
using Utils;
using Linguistics.Core.Morpho;

namespace SurfaceSyntaxAnalyzer.Homogeneous
{
	/// <summary>
	/// Формат по выделению однородных членов
	/// </summary>
	internal abstract class HomogeneousFormatBase
	{
		/// <summary>
		/// формат однородных членов с минимум 2-мя однородными
		/// </summary>
		private const string _formatWithMinTwoHomogeneous = "{0}( , {0})+( conj (etc|({0})))?";
		/// <summary>
		/// формат однородных членов с союзом перед последним элементом
		/// </summary>
		private const string _formatWithConjunction = "{0}( , {0})* ((conj etc)|(conj ({0}))|(, also ({0})))";
		/// <summary>
		/// формат однородных членов без союзов
		/// </summary>
		private const string _formatWithoutConjunction = "{0}( , {0}){{{1},}}";
		/// <summary>
		/// формат однородных членов, перед каждым элементом которой стоит союз
		/// </summary>
		private const string _formatWithEveryConjunction = "conj {0}( , conj {0})+";

		/// <summary>
		/// Закодированный юнит
		/// </summary>
		private struct CodedUnit
		{
			/// <summary>
			/// Юнит
			/// </summary>
			public UnitTextBase Unit { get; set; }
			/// <summary>
			/// Позиция
			/// </summary>
			public int Position { get; set; }
		}

		/// <summary>
		/// Кодер/декодер юнитов
		/// </summary>
		private static UnitCoderBase _coder = new HomogeneousUnitCoder();

		/// <summary>
		/// регулярное выражение
		/// </summary>
		private Regex _regex;

		#region [.ctor().]
		/// <summary>
		/// Конструктор для цепочки, в которой элемент употребляется вместо союзов
		/// </summary>
		/// <param name="item">элемент списка однородных членов</param>
		public HomogeneousFormatBase(string item)
		{
			_regex = new Regex(string.Format(_formatWithMinTwoHomogeneous, item));
		}

		/// <summary>
		/// Конструктор
		/// </summary>
		/// <param name="item">элемент списка однородных членов</param>
		/// <param name="minimumHomogeneousCount">минимальное число однородных членов без союзов</param>
		public HomogeneousFormatBase(string item, int minimumHomogeneousCount)
		{
			_regex = new Regex(string.Format("(" + _formatWithEveryConjunction + ")|("
				+ _formatWithConjunction + ")|(" + _formatWithoutConjunction + ")", item, minimumHomogeneousCount - 1));
		}
		#endregion

		/// <summary>
		/// Проверка, что заданная сущность является элементом цепочки однородных членов
		/// </summary>
		/// <param name="entity">сущность</param>
		/// <returns>результат проверки</returns>
		public abstract bool IsHomogeneousItem(Entity entity);

		/// <summary>
		/// Инициализация цепочки однородных членов
		/// </summary>
		public virtual void InitializeChain()
		{
		}

		/// <summary>
		/// Финальная корректировка коллекции элементов цепочки однородных членов
		/// </summary>
		/// <param name="collection">коллекция элементов цепочки однородных членов</param>
		public virtual void FinalCorrectChain(IList<UnitTextBase> collection)
		{
		}

		/// <summary>
		/// Проверка, что заданный юнит допустим для цепочки однородных членов
		/// </summary>
		/// <param name="unit">юнит</param>
		/// <returns>результат проверки</returns>
		public virtual bool IsAvailable(UnitTextBase unit)
		{
			return true;
		}

		/// <summary>
		/// Замена юнитов в коллекции на цепочки однородных сущностей
		/// </summary>
		/// <param name="unitCollection">коллекция юнитов</param>
		public void ReplaceWithHomogeneous(IList<UnitTextBase> collection)
		{
			string codedString = _coder.CodeUnits(collection);

			int startAt = 0;
			Match match = null;
			while ((match = _regex.Match(codedString, startAt)).Success)
			{
				var chain = new HomogeneousChain(_coder.GetUnits(match.Index, match.Length), this);
				if (chain.Success)
				{
					chain.ReplaceUnits(collection);
					startAt = match.Index + match.Length;
				}
				else
					startAt = match.Index + 1;
			}
		}
	}
}
