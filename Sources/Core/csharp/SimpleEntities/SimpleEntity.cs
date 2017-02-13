using System.Collections.Generic;

namespace SimpleEntities
{
	/// <summary>
	/// Простая сущность
	/// </summary>
	public class SimpleEntity
	{
		/// <summary>
		/// Тип
		/// </summary>
		public string Type { get; private set; }
		/// <summary>
		/// Текст
		/// </summary>
		public string Text { get; private set; }
		/// <summary>
		/// Значение
		/// </summary>
		public string Value { get; set; }
		/// <summary>
		/// Начальная позиция
		/// </summary>
		public int StartPosition { get; set; }
		/// <summary>
		/// Длина сущности
		/// </summary>
		public int Length { get { return Text.Length; } }
		/// <summary>
		/// Дочерние сущности
		/// </summary>
		public List<SimpleEntity> Children { get; private set; }

		#region [.ctor().]
		public SimpleEntity(string text, int startPosition, string type)
		{
			Text = text;
			Value = Text;
			StartPosition = startPosition;
			Type = type;
			Children = new List<SimpleEntity>();
		}
		#endregion

		/// <summary>
		/// Добавление дочерней сущности
		/// </summary>
		/// <param name="entity">дочерняя сущность</param>
		public void AddChild(SimpleEntity entity)
		{
			int index = -1;
			for (index = 0; index < Children.Count; ++index)
				if (entity.StartPosition < Children[index].StartPosition)
					break;
			Children.Insert(index, entity);
		}

		/// <summary>
		/// Проверка, что указанная сущность пересекается с данной
		/// </summary>
		/// <param name="entity">сущность</param>
		/// <returns>результат проверки</returns>
		public bool IsIntersect(SimpleEntity entity)
		{
            //Если this расположен левее entity
            if (StartPosition <= entity.StartPosition)
                return IsPositionInsideEntity(entity.StartPosition);
            return entity.IsPositionInsideEntity(StartPosition);

            //Старая версия
            //return IsPositionInsideEntity(entity.StartPosition) || IsPositionInsideEntity(entity.StartPosition + entity.Length - 1) ||
             //   entity.IsPositionInsideEntity(StartPosition) || entity.IsPositionInsideEntity(StartPosition + Length - 1);
		}

		/// <summary>
		/// Проверка, что указанная позиция попадает внутрь сущности
		/// </summary>
		/// <param name="position">позиция</param>
		/// <returns>результат проверки</returns>
		private bool IsPositionInsideEntity(int position)
		{
			return (StartPosition <= position) && (position < StartPosition + Length);
		}

        public override string ToString()
        {
            return (Type + "; " + Text);
        }

        /// <summary>
        /// Добавить список детей. 
        /// Дети в списке расположены по возрастанию их позиций в исходной строке.
        /// </summary>
        /// <param name="entitiesWithCurrentTypeToAdd">Список детей для добавления</param>
	    public void AddSortedChildRange(IEnumerable<SimpleEntity> entitiesWithCurrentTypeToAdd)
	    {
            int index = 0;
            foreach (SimpleEntity entityToAdd in entitiesWithCurrentTypeToAdd)
            {
                for (; index < Children.Count; ++index)
                    if (entityToAdd.StartPosition < Children[index].StartPosition)
                        break;
                Children.Insert(index, entityToAdd);
                ++index;
            }
	    }
	}
}
