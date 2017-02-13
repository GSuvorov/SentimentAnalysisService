using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Collections.ObjectModel;

namespace Linguistics.Core
{
	/// <summary>
	/// Класс для хранения дерева сущностей
	/// </summary>
    [Serializable]
	public class EntitySortedTree
	{
		/// <summary>
		/// Список вершин дерева
		/// </summary>
		private List< Entity > _headList;

		#region [.ctor().]
		public EntitySortedTree()
		{
			_headList = new List<Entity>();
		}
		#endregion

		/// <summary>
		/// Добавление сущности
		/// </summary>
		/// <param name="entity">сущность</param>
		/// <param name="isReplaceIntersected">флаг, что заданная сущность заменяет пересекающиеся с ней</param>
		public void AddEntity(Entity entity, bool isReplaceIntersected = false)
		{
			#region [.defense.]
			if (entity == null)
				throw new ArgumentNullException();
			#endregion

			int index = -1;
			for (index = 0; index < _headList.Count; ++index)
			{
				Entity head = _headList[index];
				if (head.IsContain(entity))
				{
					head.AddChild(entity);
					return;
				}
				else if (entity.IsContain(head) && !isReplaceIntersected)
				{
					entity.AddChild(head);
					RemoveHead(ref index);
				}
				else if (head.IsIntersect(entity))
				{
					if (isReplaceIntersected)
						RemoveHead(ref index);
					else
						return;
				}
				else if (entity.PositionInfo.End <= head.PositionInfo.Start)
					break;
			}
			_headList.Insert(index, entity);
		}

		/// <summary>
		/// Удаление сущности по указанному индексу.
		/// После выполнения индекс указывает на элемент перед удаленной вершиной
		/// </summary>
		/// <param name="index">индекс</param>
		private void RemoveHead(ref int index)
		{
			_headList.RemoveAt(index);
			--index;
		}

		/// <summary>
		/// Удаление сущности из списка вершин вместе со всеми ее дочерними сущностями
		/// </summary>
		/// <param name="entity">удаляемая сущность</param>
		public void RemoveEntity(Entity entity)
		{
			_headList.Remove(entity);
		}

		/// <summary>
		/// Очистка списка вершин
		/// </summary>
		public void Clear()
		{
			_headList.Clear();
		}

		/// <summary>
		/// Возвращает коллекцию дочерних элементов
		/// </summary>
		/// <returns>коллекция дочерних элементов</returns>
        public Entity[] Elements
        {
            get { return _headList.ToArray(); }
        }
	}
}
