using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Xml.Linq;
using Linguistics.Core.TonalityMarking;

namespace Linguistics.Core
{
	/// <summary>
	/// Подпредложение
	/// </summary>
    [Serializable]
	public class SubSentence : IRelations< SubSentence, Sentence >, ISerializerToRDF
	{
		#region [IRelationsBase]
		/// <summary>
		/// Предыдущее подпредложение
		/// </summary>
		public SubSentence Previous { get; set; }
		/// <summary>
		/// Следующее подпредложение
		/// </summary>
		public SubSentence Next { get; set; }
		/// <summary>
		/// Текущее подпредложение
		/// </summary>
		public SubSentence Current { get { return this; } }
		/// <summary>
		/// Родительское предложение
		/// </summary>
		public Sentence ParentObject { get; set; }
		#endregion

		#region [.SubSentence properties.]
		/// <summary>
		/// Единицы текста
		/// </summary>
		/// должен быть всегда заполнен
        public UnitTextBase[] Units { get; private set; }
		/// <summary>
		/// Дочерние подпредложения
		/// </summary>
		/// Не может быть null
		public SubSentence[] Children { get; private set; }

		/// <summary>
		/// Подпредложение, в котором лежит субъект текущем подпредложения
		/// </summary>
		private SubSentence _subjectSubSentence;
		/// <summary>
		/// Субъект подпредложения, лежащий в текущем подпредложении
		/// </summary>
		private Entity _subjectInCurrent;
		/// <summary>
		/// Субъект подпредложения
		/// </summary>
		public Entity Subject
		{
			get { return (_subjectSubSentence != null) ? _subjectSubSentence.Subject : _subjectInCurrent; }
			set
			{
				_subjectInCurrent = value;
				_subjectSubSentence = null;
			}
		}
		/// <summary>
		/// Установка подпредложения, в котором лежит субъект текущего подпредложения
		/// </summary>
		/// <param name="subSentence"></param>
		public void set_SubjectSubSentence( SubSentence subSentence )
		{
			_subjectSubSentence = subSentence;
			_subjectInCurrent = null;
		}
		#endregion

		#region [.self properties.]
		/// <summary>
		/// Тип подпредложения
		/// </summary>
		public SubSentenceType Type { get; set; }

		/// <summary>
		/// Начальная позиция подпредложения
		/// </summary>
		public int StartPosition { get; private set; }

		/// <summary>
		/// Информация о подтексте
		/// </summary>
		public SubTextInfo SubTextInfo { get; set; }

        /// <summary>
        /// Пассивный субъект
        /// </summary>
        public bool? IsPassiveSubject { get; set; }
		#endregion

		#region [.ctor().]
		public SubSentence(SubSentenceType type, int startPosition)
		{
			Type = type;
			Children = new SubSentence[]{};
			StartPosition = startPosition;
		}
		#endregion

		/// <summary>
		/// Установка текущей коллекции юнитов
		/// </summary>
		/// <param name="units">коллекция юнитов</param>
		public void SetUnits( IEnumerable< UnitTextBase > units )
		{
			Units = units.OrderBy(_ => _.PositionInfo.Start).ToArray();
			Units.SetRelations(this);
		}

		/// <summary>
		/// Добавление подпредложений к уже имеющимся
		/// </summary>
		/// <param name="subSentences">коллекция подпредложений</param>
		public void AppendSubSentences(IEnumerable<SubSentence> subSentences)
		{
			var children = subSentences.ToArray();
			if ((Children != null) && Children.Any())
				children = DecomposedChildren(Children.Union(subSentences));
			SetSubSentences(children);
		}

		/// <summary>
		/// Установка текущей коллекции подпредложений
		/// </summary>
		/// <param name="subSentences">коллекция подпредложений</param>
		private void SetSubSentences(IEnumerable<SubSentence> subSentences)
		{
			Children = subSentences.OrderBy(_ => _.StartPosition).ToArray();
			Children.SetRelations(ParentObject);
			UpdateStartPosition();
		}

		/// <summary>
		/// Распределение подпредложений по наследникам друг к другу
		/// </summary>
		/// <param name="subSentences">коллекция подпредложений</param>
		/// <returns>коллекций подпредложений верхнего уровня</returns>
		private SubSentence[] DecomposedChildren(IEnumerable<SubSentence> subSentences)
		{
			var result = subSentences.OrderBy(_ => _.StartPosition).ToList();
			for (int i = 0; i < result.Count; ++i)
			{
				var child = result[i];
				if (child.SubTextInfo != null)
					continue;

				int endPosition = child.Units.Any() ? child.Units.Last().PositionInfo.End : child.StartPosition;
				child.AppendSubSentences(TakeChildren(result, i + 1, endPosition));
			}
			return result.ToArray();
		}

		/// <summary>
		/// Выборка из коллекции дочерних подпредложений
		/// </summary>
		/// <param name="list">список подпредложений</param>
		/// <param name="startIndex">стартовый индекс для выборки</param>
		/// <param name="position">позиция, раньше которой должны начинаться дочерние подпредложения</param>
		/// <returns>дочерние подпредложения</returns>
		private SubSentence[] TakeChildren(List<SubSentence> list, int startIndex, int position)
		{
			List<SubSentence> result = new List<SubSentence>();
			int index = startIndex;
			for (int i = startIndex; i < list.Count; ++i)
			{
				if (list[i].StartPosition < position)
				{
					result.Add(list[i]);
					list.RemoveAt(i);
					--i;
				}
				else
					break;
			}
			return result.ToArray();
		}

		/// <summary>
		/// Обновление начальной позиции подпредложения на основе дочерних подпредложений
		/// </summary>
		private void UpdateStartPosition()
		{
			foreach (SubSentence child in Children)
				if (child.StartPosition < StartPosition)
					StartPosition = child.StartPosition;
		}

		#region [.correct units.]
		/// <summary>
		/// Добавление заданного юнита перед текущим
		/// </summary>
		/// <param name="current">текущий юнит</param>
		/// <param name="unit">добавляемый юнит</param>
		public void AddBeforeCurrent(UnitTextBase current, UnitTextBase unit)
		{
			CorrectUnits(current, AddBeforeCurrent, unit);
		}

		/// <summary>
		/// Добавление заданного юнита после текущего
		/// </summary>
		/// <param name="current">текущий юнит</param>
		/// <param name="unit">добавляемый юнит</param>
		public void AddAfterCurrent(UnitTextBase current, UnitTextBase unit)
		{
			CorrectUnits(current, AddAfterCurrent, unit);
		}

		/// <summary>
		/// Удаление текущего юнита
		/// </summary>
		/// <param name="current">текущий юнит</param>
		public void RemoveCurrent(UnitTextBase current)
		{
			CorrectUnits(current, RemoveCurrent);
		}

		/// <summary>
		/// Замена в предложении текущего юнита заданными
		/// </summary>
		/// <param name="current">текущий юнит</param>
		/// <param name="units">юниты для замены</param>
		public void ReplaceCurrentWith(UnitTextBase current, params UnitTextBase[] units)
		{
			CorrectUnits(current, ReplaceCurrentWith, units);
		}

		/// <summary>
		/// Преобразователь списка юнитов с сохранением связей
		/// </summary>
		/// <param name="collection">список юнитов</param>
		/// <param name="current">текущий юнит</param>
		/// <param name="units">юниты для корректировки списка</param>
		private delegate void Corrector(List<UnitTextBase> collection, UnitTextBase current, params UnitTextBase[] units);

		/// <summary>
		/// Добавление заданных юнитов перед текущим
		/// </summary>
		/// <param name="collection">список юнитов</param>
		/// <param name="current">текущий юнит</param>
		/// <param name="units">коллекция добавляемых юнитов</param>
		private void AddBeforeCurrent(List<UnitTextBase> collection, UnitTextBase current, params UnitTextBase[] units)
		{
			units.SetRelations(this);
			int index = GetIndex(collection, current);
			if (index > 0)
				units.First().SetRelation(collection[index - 1]);
			current.SetRelation(units.Last());
			collection.InsertRange(index, units);
		}

		/// <summary>
		/// Добавление заданных юнитов после текущего
		/// </summary>
		/// <param name="collection">список юнитов</param>
		/// <param name="current">текущий юнит</param>
		/// <param name="units">коллекция добавляемых юнитов</param>
		private void AddAfterCurrent(List<UnitTextBase> collection, UnitTextBase current, params UnitTextBase[] units)
		{
			units.SetRelations(this);
			int index = GetIndex(collection, current) + 1;
			if (index < collection.Count)
				collection[index].SetRelation(units.Last());
			units.First().SetRelation(current);
			collection.InsertRange(index, units);
		}

		/// <summary>
		/// Удаление текущего юнита
		/// </summary>
		/// <param name="collection">список юнитов</param>
		/// <param name="current">текущий юнит</param>
		/// <param name="units">не используется</param>
		private void RemoveCurrent(List<UnitTextBase> collection, UnitTextBase current, params UnitTextBase[] units)
		{
			int index = GetIndex(collection, current);
			UnitTextBase previous = (index > 0) ? collection[index - 1] : null;
			UnitTextBase next = (index < collection.Count - 1) ? collection[index + 1] : null;
			if (previous != null)
				previous.Next = next;
			if (next != null)
				next.Previous = previous;
			collection.RemoveAt(index);
		}

		/// <summary>
		/// Замена текущего юнита заданными
		/// </summary>
		/// <param name="collection">список юнитов</param>
		/// <param name="current">текущий юнит</param>
		/// <param name="units">коллекция юнитов для замены</param>
		private void ReplaceCurrentWith(List<UnitTextBase> collection, UnitTextBase current, params UnitTextBase[] units)
		{
			AddAfterCurrent(collection, current, units);
			RemoveCurrent(collection, current);
		}

		/// <summary>
		/// Корректировка коллекции юнитов подпредложения
		/// </summary>
		/// <param name="current">текущий юнит</param>
		/// <param name="corrector">преобразователь</param>
		/// <param name="units">юниты для корректировки</param>
		private void CorrectUnits(UnitTextBase current, Corrector corrector, params UnitTextBase[] units)
		{
			if (units.Any())
			{
				List<UnitTextBase> collection = Units.ToList();
				corrector(collection, current, units);
				Units = collection.ToArray();
			}
		}

		/// <summary>
		/// Получение индекса заданного юнита в коллекции.
		/// Юнит должен быть в коллекции
		/// </summary>
		/// <param name="collection">коллекция юнитов</param>
		/// <param name="unit">юнит для поиска</param>
		/// <returns>индекс юнита</returns>
		private int GetIndex(List<UnitTextBase> collection, UnitTextBase unit)
		{
			int result = collection.IndexOf(unit);
			if (result < 0)
				throw new ArgumentException("collection not contain unit");
			return result;
		}
		#endregion

		#region [ISerializerToRDF]
		public XElement ToXElement()
		{
            var result = new XElement( UnitTextType.SUB_SENT.ToString() );
			result.SetAttributeValue(RDF.Attribute.Type, Type);
			result.SetAttributeValue(RDF.Attribute.ID, IdGenerator.GetID(this));
			if (SubTextInfo != null)
			{
				var subText = SubTextInfo.ToXElement();
				foreach (var element in subText.Elements())
					if (element.IsContainAttribute(RDF.Attribute.StartPosition))
						SubTextInfo.UpdateStartPosition(element, StartPosition);
				result.Add(subText.Elements());
				return result;
			}

			SetSubjectAttribute(result);
            SetPassiveSubjectAttribute(result);
            #region [.Тональность.]
            if ( Tonality.SNT != TonalityType.__UNDEFINE__ )
            {
				result.SetAttributeValue(RDF.Attribute.SNT, Tonality.SNT);
				result.SetAttributeValue(RDF.Attribute.FRT, Tonality.FRT);
            }
            if ( Tonality.SNTFINAL != TonalityType.__UNDEFINE__ )
            {
				result.SetAttributeValue(RDF.Attribute.SNTFINAL, Tonality.SNTFINAL);
				result.SetAttributeValue(RDF.Attribute.FRTFINAL, Tonality.FRTFINAL);
            }
            #endregion
			AddChildElements(result);

            return result;
		}

		/// <summary>
		/// Установка атрибута подлежащего к заданному элементу, если это необходимо
		/// </summary>
		/// <param name="element">элемент</param>
		private void SetSubjectAttribute(XElement element)
		{
			if (Subject != null)
			{
				int id = 0;
				if (IdGenerator.GetExistedID(Subject, out id))
					element.SetAttributeValue(RDF.Attribute.Subject, id);
				#region [.defense.]
				else
					throw new InvalidOperationException("Entity without ID");
				#endregion
			}
		}

        /// <summary>
        /// Установка атрибута пассивного субъекта к заданному элементу, если это необходимо
        /// </summary>
        /// <param name="element">элемент</param>
        private void SetPassiveSubjectAttribute(XElement element)
        {
            if (IsPassiveSubject.HasValue)
                element.SetAttributeValue(RDF.Attribute.IsPassiveSubject, IsPassiveSubject.Value.ToString().ToUpper());
        }

		/// <summary>
		/// Добавление дочерних элементов для заданного элемента
		/// </summary>
		/// <param name="element">заданный элемент</param>
		private void AddChildElements(XElement element)
		{
			int childIndex = 0;
			for (int i = 0; i < Units.Length; ++i)
			{
				while ((childIndex < Children.Length) &&
						(Children[childIndex].StartPosition < Units[i].PositionInfo.Start)
					  )
				{
					element.Add(Children[childIndex].ToXElement());
					++childIndex;
				}
				if ((i != 0) || (Units[i].Text.Length > 0))
					element.Add(Units[i].ToXElement());
			}
			for (; childIndex < Children.Length; ++childIndex)
				element.Add(Children[childIndex].ToXElement());
		}
		#endregion

        #region [.Advanced properties.]
        private Entity[] _Entities;
        /*public Entity[] Entities
        {
            get
            {
                if ( _Entities == null )
                {
                    var lst = new List< Entity >( Units.Length );
                    for ( int i = 0; i < Units.Length; i++ )
                    {
                        var unit = Units[ i ];
                        if ( unit.IsEntity )
                        {
                            lst.Add( (Entity) unit );
                        }
                    }
                    _Entities = lst.ToArray();
                }
                return (_Entities);
            }
        }*/
        public Entity[] Entities
        {
            get
            {
                if ( _Entities == null )
                {
                    if ( Type == SubSentenceType.DirectSpeech )
                    {
                        _Entities = new Entity[ 0 ];
                    }
                    else
                    {
                        var lst = new List< Entity >( Units.Length );
                        for ( int i = 0; i < Units.Length; i++ )
                        {
                            var unit = Units[ i ];
                            if ( unit.IsEntity )
                            {
                                lst.Add( (Entity) unit );
                            }
                        }
                        _Entities = lst.ToArray();
                    }
                }
                return (_Entities);
            }
        }

        /*
        private Entity[] _DescendantsEntities;
        public Entity[] DescendantsEntities
        {
            get
            {
                if ( _DescendantsEntities == null )
                {
                    var lst = new List< Entity >( Entities.Length );
                    for ( int i = 0; i < Entities.Length; i++ )
                    {
                        var entity = Entities[ i ];
                        
                        lst.AddRange( entity.d );
                    }
                    _DescendantsEntities = lst.ToArray();
                }
                return (_DescendantsEntities);
            }
        }
        */
        #endregion

        #region [.Тональность.]
        private TonalityInfoSubSentence _Tonality;
        public TonalityInfoSubSentence Tonality
        {
            get 
            {
                if ( _Tonality == null )
                {
                    _Tonality = new TonalityInfoSubSentence();
                }
                return (_Tonality); 
            }
        }
        #endregion
    }
}
