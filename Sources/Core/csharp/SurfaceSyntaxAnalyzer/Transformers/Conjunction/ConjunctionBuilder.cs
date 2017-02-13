using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Linguistics.Core;

namespace SurfaceSyntaxAnalyzer
{
	/// <summary>
	/// Строитель союза
	/// </summary>
	internal class ConjunctionBuilder
	{
		/// <summary>
		/// строитель значения союза
		/// </summary>
		private StringBuilder _conjunctionValue;

		private Conjunction _conjunction;
		/// <summary>
		/// союз, который сейчас строится
		/// </summary>
		public Conjunction Conjunction
		{
			get { return _conjunction; }
			set
			{
				_conjunction = value;
				_conjunctionValue = new StringBuilder();
			}
		}

		#region [.ctor().]
		public ConjunctionBuilder()
		{
		}
		#endregion

		/// <summary>
		/// Добавление в конец заданной строки начиная с заданной начальной позиции
		/// </summary>
		/// <param name="text">строка</param>
		/// <param name="startPosition">начальная позиция</param>
		/// <returns>индекс конца строки</returns>
		public int AppendConjunctionPart(string text, int startPosition)
		{
			if (Conjunction == null)
				throw new InvalidOperationException("Conjunction is null");
			
			if (startPosition != 0)
				_conjunctionValue.Append(text.Substring(startPosition, text.Length - startPosition));
			else
				_conjunctionValue.Append(text);
			return text.Length;
		}

		/// <summary>
		/// Добавление в заданную коллекию юнитов левой границы союза
		/// </summary>
		/// <param name="unitCollection">коллекция юнитов</param>
		/// <param name="unit">юнит, в котором начинается союз</param>
		/// <param name="startPosition">начальная позиция в тексте юнита</param>
		/// <returns>позиция в тексте юнита, с которой идет союз</returns>
		public int AddLeftConjunctionBorder(List<UnitTextBase> unitCollection, UnitTextBase unit, int startPosition)
		{
			int conjunctionStartPosition = Conjunction.StartPosition - unit.PositionInfo.Start;
			unitCollection.Add(unit.CreateUnmarkedText(startPosition, conjunctionStartPosition));
			return conjunctionStartPosition;
		}

		/// <summary>
		/// Добавление в заданную коллекцию юнитов союза и очистка внутреннего состояния
		/// </summary>
		/// <param name="unitCollection">коллекция юнитов</param>
		/// <param name="unit">>юнит, в котором заканчивается союз</param>
		/// <param name="startPosition">начальная позиция в тексте юнита</param>
		/// <returns>позиция в тексте юнита, где закончился союз</returns>
		public int AddAndClearConjunction(List<UnitTextBase> unitCollection, UnitTextBase unit, int startPosition)
		{
			int endPosiiton = Conjunction.EndPosition - unit.PositionInfo.Start;
			_conjunctionValue.Append(unit.CreateUnmarkedText(startPosition, endPosiiton).Text);
			
			var conjunctionEntity = CreateNewConjunctionEntity(_conjunctionValue.ToString(), Conjunction, unit);
			unitCollection.Add(conjunctionEntity);

			Clear();
			return endPosiiton;
		}

		/// <summary>
		/// Создание новой сущности с типом союз
		/// </summary>
		/// <param name="text">текст</param>
		/// <param name="conjunction">союз</param>
		/// <param name="lastUnit">последний юнит союза</param>
		/// <returns>новая сущность</returns>
		private Entity CreateNewConjunctionEntity(string text, Conjunction conjunction, UnitTextBase lastUnit)
		{
			var info = new ConjunctionEntityInfo(conjunction.Type) { Value = conjunction.Value };
			if (lastUnit.IsEntity && (conjunction.Value == lastUnit.Text))
			{
				Entity conjunctionEntity = (Entity)lastUnit;
				info.Morpho = conjunctionEntity.EntityInfo.Morpho;
				info.Value = conjunctionEntity.EntityInfo.Value;
			}
			return new Entity(text, conjunction.StartPosition, info);
		}

		/// <summary>
		/// Очистка внутренннего состояния
		/// </summary>
		public void Clear()
		{
			_conjunction = null;
		}
	}
}
