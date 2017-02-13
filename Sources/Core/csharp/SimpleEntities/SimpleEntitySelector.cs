using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Text;
using System.IO;
using System.Xml.Linq;
using System.Text.RegularExpressions;

namespace SimpleEntities
{
	/// <summary>
	/// Класс выделения сущностей по регулярным выражениям
	/// </summary>
	public class SimpleEntitySelector
	{
		/// <summary>
		/// Регулярные выражения для выделения простых сущностей
		/// </summary>
		private readonly SimpleEntityResource _expressions;

		#region [.ctor().]
		public SimpleEntitySelector(string path, string fileName)
		{
			_expressions = new SimpleEntityResource(path, fileName);
		}

		public SimpleEntitySelector(string path)
		{
			_expressions = new SimpleEntityResource(path, "entity.xml");
		}
		#endregion

		/// <summary>
		/// Выделение из текста сущностей по регулярным выражениям
		/// </summary>
		/// <param name="text">текст</param>
		/// <returns>выделенные сущности</returns>
		public SimpleEntity[] SelectSimpleEntities(string text)
		{
			SimpleEntity textEntity = new SimpleEntity(text, 0, "");
			SelectEntities(textEntity, _expressions.GetAllTypes());
			return textEntity.Children.ToArray();
		}

		/// <summary>
		/// Выделение из текста сущностей по заданному регулярному выражению
		/// </summary>
		/// <param name="text">текст</param>
		/// <param name="expression">регулярное выражение</param>
		/// <param name="type">тип сущности</param>
		/// <returns>выделенные сущности</returns>
		public SimpleEntity[] SelectSimpleEntities(string text, Regex expression, string type)
		{
			SimpleEntity textEntity = new SimpleEntity(text, 0, "");
			EntityExpression entityExpression = new EntityExpression(expression) { EntityType = type };
			UseExpression(textEntity, entityExpression);
			return textEntity.Children.ToArray();
		}

		private void SelectEntities(SimpleEntity mainEntity, IEnumerable<string> expressions)
		{
            //сущности текущего типа, подлежащие добавлению в mainEntity
		    List < SimpleEntity > entitiesWithCurrentTypeToAdd = new List<SimpleEntity>();
			bool isEmptyMain = string.IsNullOrEmpty(mainEntity.Type);
			foreach (string type in expressions)
			{
				EntityExpression expression = _expressions.GetExpression(type);
				if ((expression.SelectionType == SelectionType.False) ||
					(isEmptyMain && (expression.SelectionType == SelectionType.Child)))
				{
					continue;
				}

				UseExpression(mainEntity, expression);
			}
			foreach (SimpleEntity entity in mainEntity.Children)
				SelectEntities(entity, _expressions.GetExpression(entity.Type).UsedExpressions);
		}

		/// <summary>
		/// Применение выражения
		/// </summary>
		/// <param name="mainEntity">родительская сущность</param>
		/// <param name="expression">выражение</param>
		private void UseExpression(SimpleEntity mainEntity, EntityExpression expression)
		{
			//сущности текущего типа, подлежащие добавлению в mainEntity
			List<SimpleEntity> entitiesWithCurrentTypeToAdd = new List<SimpleEntity>();
			MatchCollection matches = Regex.Matches(mainEntity.Text, expression.Expression, RegexOptions.Singleline);
			foreach (Match match in matches)
				AddNewEntity(mainEntity, match, expression.EntityType, entitiesWithCurrentTypeToAdd);
			mainEntity.AddSortedChildRange(entitiesWithCurrentTypeToAdd);
		}

		/// <summary>
		/// Добавление найденной сущности в список выделенных сущностей.
		/// Если сущность пересекается с одной или несколькими ранее выделенными сущностями,
		/// то выбирается более длинная цепочка
		/// </summary>
		/// <param name="mainEntity">родительская сущность</param>
		/// <param name="match">найденное соответствие</param>
		/// <param name="type">тип сущности</param>
        /// <param name="entitiesWithCurrentTypeToAdd">сущности текущего типа, подлежащие добавлению в mainEntity</param>
		/// <returns></returns>
        private void AddNewEntity(SimpleEntity mainEntity, Match match, string type, List<SimpleEntity> entitiesWithCurrentTypeToAdd)
		{
			Group valueGroup = SimpleEntityResource.GetValueGroup(match);

			SimpleEntity entity = new SimpleEntity(valueGroup.Value, valueGroup.Index, type);
            List<SimpleEntity> intersections = GetIntersections(mainEntity, entity);

			if (IsNeedAddEntity(entity, intersections))
			{
                foreach (SimpleEntity intersectEntity in intersections)
                    mainEntity.Children.Remove(intersectEntity);
                entitiesWithCurrentTypeToAdd.Add(entity);
                //mainEntity.AddChild(entity);
			}
		}

        /// <summary>
        /// Получить пересечения
        /// </summary>
        /// <param name="mainEntity"></param>
        /// <param name="entity"></param>
        /// <returns></returns>
	    private List<SimpleEntity> GetIntersections(SimpleEntity mainEntity, SimpleEntity entity)
	    {
	        return GetIntersectionBinaryAlgorithm(mainEntity, entity);
	    }

        /// <summary>
        /// Получить пересечения, используя бинарный поиск.
        /// </summary>
        /// <param name="mainEntity"></param>
        /// <param name="entity"></param>
        /// <returns></returns>
	    private List<SimpleEntity> GetIntersectionBinaryAlgorithm(SimpleEntity mainEntity, SimpleEntity entity)
	    {
	        List<SimpleEntity> children = mainEntity.Children;
            int nearestChildIndex = FindNearestChildIndex(entity, children);
            return GetIntersectionsUsingNearestChild(entity, children, nearestChildIndex);
	    }

        /// <summary>
        /// Получить пересечения, используя индекс Child'а, ближайшего к месту вставки entity.
        /// </summary>
        /// <param name="entity">Entity для вставки</param>
        /// <param name="children">Дети</param>
        /// <param name="nearestChildIndex">индекс Child'а, ближайшего к месту вставки</param>
        /// <returns>Пересечения</returns>
	    private static List<SimpleEntity> GetIntersectionsUsingNearestChild(SimpleEntity entity, List<SimpleEntity> children, int nearestChildIndex)
	    {
	        List<SimpleEntity> intersections = new List<SimpleEntity>();
	        int goToRight = nearestChildIndex + 1;
	        int goToLeft = nearestChildIndex - 1;

	        if (children.Count != 0 && entity.IsIntersect(children[nearestChildIndex]))
	            intersections.Add(children[nearestChildIndex]);

	        for (int i = goToRight; i < children.Count; ++i)
	        {
	            SimpleEntity simpleEntity = children[i];
	            if (!entity.IsIntersect(simpleEntity))
	                break;
	            intersections.Add(simpleEntity);
	        }

	        for (int i = goToLeft; i > 0; --i)
	        {
	            SimpleEntity simpleEntity = children[i];
	            if (!entity.IsIntersect(simpleEntity))
	                break;
	            intersections.Add(simpleEntity);
	        }
	        return intersections;
	    }

        /// <summary>
        /// Поиск индекса Child'а, ближайшего к месту вставки entity
        /// </summary>
        /// <param name="entity">Entity для вставки</param>
        /// <param name="children">Дети</param>
        /// <returns>индекса Child'а, ближайшего к месту вставки entity</returns>
	    private static int FindNearestChildIndex(SimpleEntity entity, List<SimpleEntity> children)
	    {
	        int position = entity.StartPosition;
	        int leftBorder = 0;
	        int rightBorder = children.Count;

	        int middle = 0;
	        while (leftBorder < rightBorder)
	        {
	            middle = leftBorder + (rightBorder - leftBorder)/2;
	            if (position <= children[middle].StartPosition)
	                rightBorder = middle;
	            else
	                leftBorder = middle + 1;
	        }
	        return middle;
	    }

	    /// <summary>
		/// Проверка, что данную сущность требуется добавить
		/// </summary>
		/// <param name="entity">сущность</param>
		/// <param name="intersections">пересекающиеся с ней сущности</param>
		/// <returns>результат проверки</returns>
		private bool IsNeedAddEntity(SimpleEntity entity, List<SimpleEntity> intersections)
		{
			bool result = true;
			foreach (SimpleEntity ent in intersections)
			{
                if (entity.Length == ent.Length)
                    result = result && !_expressions.GetExpression(ent.Type).UsedExpressions.Contains(entity.Type);
                else if (entity.Length < ent.Length)
                    result = false;
			}
			return result;
		}
	}
}