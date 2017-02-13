using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Xml.Linq;

namespace Linguistics.Core
{
	/// <summary>
	/// Базовый класс единицы текста
	/// </summary>
    [Serializable]
	public abstract class UnitTextBase : IRelations< UnitTextBase, SubSentence >, ISerializerToRDF
	{
		/// <summary>
		/// Текстовое значение
		/// </summary>
		public string Text { get; protected set; }

		/// <summary>
		/// Тип
		/// </summary>
		public abstract UnitTextType UnitTextType { get; }

		/// <summary>
		/// Флаг сущности
		/// </summary>
		public virtual bool IsEntity { get { return false; } }

		#region [.position's.]
		/// <summary>
		/// Информации о положении юнита
		/// </summary>
        [Serializable]
		protected struct UnitPositionInfo : IUnitPositionInfo
		{
			public int Start { get; set; }
			public int End { get; set; }
		}

		/// <summary>
		/// Получение информации о положении юнита в тексте
		/// </summary>
		public IUnitPositionInfo PositionInfo { get { return _positionInfo; } }

		/// <summary>
		/// информация о положении юнита в тексте
		/// </summary>
		protected UnitPositionInfo _positionInfo;

		/// <summary>
		/// Сдвиг позиции сущности
		/// </summary>
		/// <param name="leftShift">сдвиг</param>
		public virtual void MovePosition(int leftShift)
		{
			_positionInfo.Start -= leftShift;
			_positionInfo.End -= leftShift;
		}
		#endregion

		#region [.correct unit collection.]
		/// <summary>
		/// Добавление заданного юнита в предложение перед текущим
		/// </summary>
		/// <param name="unit">добавляемый юнит</param>
		public void AddBeforeSelf(UnitTextBase unit)
		{
			ParentObject.AddBeforeCurrent(this, unit);
		}

		/// <summary>
		/// Добавление заданного юнита в предложение после текущего
		/// </summary>
		/// <param name="unit">добавляемый юнит</param>
		public void AddAfterSelf(UnitTextBase unit)
		{
			ParentObject.AddAfterCurrent(this, unit);
		}

		/// <summary>
		/// Удаление текущего юнита из предложения
		/// </summary>
		public void Remove()
		{
			ParentObject.RemoveCurrent(this);
		}

		/// <summary>
		/// Замена в предложении текущего юнита заданными
		/// </summary>
		/// <param name="units">юниты для замены</param>
		public void ReplaceWith(params UnitTextBase[] units)
		{
			ParentObject.ReplaceCurrentWith(this, units);
		}
		#endregion

		#region [IRelationsBase]
		/// <summary>
		/// Предыдущий юнит
		/// </summary>
		public UnitTextBase Previous { get; set; }
		/// <summary>
		/// Следующий юнит
		/// </summary>
		public UnitTextBase Next { get; set; }
		/// <summary>
		/// Текущий юнит
		/// </summary>
		public UnitTextBase Current { get { return this; } }

		/// <summary>
		/// Родительское подпредложение
		/// </summary>
		public virtual SubSentence ParentObject { get; set; }
		#endregion

		#region [.ctor().]
		public UnitTextBase(string text, int startPosition)
		{
			Text = text;
			_positionInfo = new UnitPositionInfo() { Start = startPosition, End = startPosition + text.Length };
		}
		#endregion		
	
		#region [ISerializerToRDF]
		public abstract XElement ToXElement();
		#endregion

		#region [.intersection.]
		/// <summary>
		/// Проверка, что указанная позиция попадает внутрь данного юнита
		/// </summary>
		/// <param name="position">позиция</param>
		/// <returns>результат проверки</returns>
		public bool IsPositionInsideEntity(int position)
		{
			return (_positionInfo.Start <= position) && (position < _positionInfo.End);
		}
		#endregion
	}
}
