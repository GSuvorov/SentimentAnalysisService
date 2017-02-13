using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Collections.ObjectModel;
using System.Xml.Linq;
using Linguistics.Core.TonalityMarking;

namespace Linguistics.Core
{
	/// <summary>
	/// Сущность
	/// </summary>
	/// раньше - ENTITY, CET, VERB
    [Serializable]
    public class Entity : UnitTextBase
	{
		/// <summary>
		/// !!!!!!!!!!!!!!!!!!!!! ВРЕМЕННОЕ СВОЙСТВО - язык предложения
		/// </summary>
		public string Language { get; set; }

        #region [.ctor().]
        public Entity( string text, int startPosition, EntityInfoBase entityInfo )
			: base( text, startPosition )
		{
			EntityInfo = entityInfo;
			IsUse = true;
			_Children = new EntitySortedTree();
			Role = EntityRole.Undefined;
		}
        #endregion

		#region [UnitTextBase]
		public override UnitTextType UnitTextType { get { return (EntityInfo.UnitTextType); } }

		public override bool IsEntity { get { return true; } }

		public override void MovePosition(int leftShift)
		{
			base.MovePosition(leftShift);
			foreach (Entity child in Children)
				child.MovePosition(leftShift);
		}

		/// <summary>
		/// Родительское подпредложение
		/// </summary>
		private SubSentence _parentSubSentence;
		public override SubSentence ParentObject
		{
			get { return (Parent != null) ? Parent.ParentObject : _parentSubSentence; }
			set { _parentSubSentence = value; }
		}
		#endregion

		#region [.self properties.]
		/// <summary>
		/// Информация о сущности
		/// </summary>
		public EntityInfoBase EntityInfo { get; set; }

		/// <summary>
		/// Тип сущности
		/// </summary>
		public EntityTypeStructure Type { get { return EntityInfo.Type; } }
		/// <summary>
		/// Нормализованное значение
		/// </summary>
		public string Value { get { return EntityInfo.Value; } }

		/// <summary>
		/// Роль сущности
		/// </summary>
		public EntityRole Role { get; set; }

        /// <summary>
		/// Флаг использования сущности
		/// </summary>
		public bool IsUse { get; set; }

		/// <summary>
		/// Проверка, что сущность является сущностью заданного тип
		/// </summary>
		/// <param name="type">тип сущности</param>
		/// <returns>результат проверки</returns>
		public bool IsType(EntityType type)
		{
			return Type.EntityType == type;
		}
		#endregion

		#region [.children entities.]
		/// <summary>
		/// Родительская сущность
		/// </summary>
		public Entity Parent { get; protected set; }

		/// <summary>
		/// Дерево дочерних сущностей
		/// </summary>
		private EntitySortedTree _Children;

		/// <summary>
		/// Синтаксически дочерние сущности
		/// </summary>
		public List<Entity> SyntaxChildren { get; set; }

		/// <summary>
		/// Возвращает коллекцию дочерних элементов
		/// </summary>
		/// <returns>коллекция дочерних элементов</returns>
		public Entity[] Children
		{
            get { return _Children.Elements; }
		}

		/// <summary>
		/// Добавление дочерней сущности
		/// </summary>
		/// <param name="entity">дочерняя сущность</param>
		public void AddChild(Entity entity)
		{
			entity.Parent = this;
			_Children.AddEntity(entity);

			#region [.defense.]
			if (_positionInfo.End < entity._positionInfo.End)
				throw new InvalidOperationException("Child not inside Parent");
			#endregion
		}

		/// <summary>
		/// Присоединение дочернего юнита к сущности
		/// </summary>
		/// <param name="unit">дочерний юнит</param>
		public void AppendChild(UnitTextBase unit)
		{
			if (_positionInfo.Start < 0)
				_positionInfo.Start = unit.PositionInfo.Start;
			_positionInfo.End = unit.PositionInfo.End;
			Text += unit.Text;
			if (unit.IsEntity)
				AddChild((Entity)unit);
		}

		/// <summary>
		/// Присоединение дочернего юнита в начало сущности
		/// </summary>
		/// <param name="unit">дочерний юнит</param>
		public void AppendChildToBegin(UnitTextBase unit)
		{
			#region [.defense.]
			if (unit.PositionInfo.End != _positionInfo.Start)
				throw new ArgumentException("isn't near unit");
			#endregion
			_positionInfo.Start = unit.PositionInfo.Start;
			Text = Text.Insert(0, unit.Text);
			if (unit.IsEntity)
				AddChild((Entity)unit);
		}

		/// <summary>
		/// Вставка текста в указанную позицию сущности
		/// </summary>
		/// <param name="text">текст</param>
		/// <param name="position">позиция</param>
		public void InsertText(string text, int position)
		{
			Text = Text.Insert(position, text);
			_positionInfo.End += text.Length;
			int textPosition = position + _positionInfo.Start;
			foreach (var child in Children)
			{
				if (child.IsPositionInsideEntity(textPosition))
					child.InsertText(text, position - (child._positionInfo.Start - _positionInfo.Start));
				else if (child._positionInfo.Start > textPosition)
					child.MovePosition(-text.Length);
			}
		}

		/// <summary>
		/// Удаление дочерней сущности
		/// </summary>
		/// <param name="entity">удаляемая сущность</param>
		public void RemoveChild(Entity entity)
		{
			entity.Parent = null;
			_Children.RemoveEntity(entity);
		}
		#endregion

		#region [.intersection.]
		/// <summary>
		/// Проверка, что указанная сущность полностью лежит в данной
		/// </summary>
		/// <param name="entity">сущность</param>
		/// <returns>результат проверки</returns>
		public bool IsContain(Entity entity)
		{
			return !IsSamePosition(entity) &&
				IsPositionInsideEntity(entity._positionInfo.Start) && IsPositionInsideEntity(entity._positionInfo.End - 1);
		}

		/// <summary>
		/// Проверка, что указанная сущность пересекается с данной
		/// </summary>
		/// <param name="entity">сущность</param>
		/// <returns>результат проверки</returns>
		public bool IsIntersect(Entity entity)
		{
			return IsPositionInsideEntity(entity._positionInfo.Start) || IsPositionInsideEntity(entity._positionInfo.End - 1) ||
				entity.IsPositionInsideEntity(_positionInfo.Start) || entity.IsPositionInsideEntity(_positionInfo.End - 1);
		}

		/// <summary>
		/// Проверка, что заданная сущность занимает одинаковое место с данной
		/// </summary>
		/// <param name="entity">сущность</param>
		/// <returns>результат проверки</returns>
		public bool IsSamePosition(Entity entity)
		{
			return (_positionInfo.Start == entity._positionInfo.Start) && (_positionInfo.End == entity._positionInfo.End);
		}
		#endregion

		#region [ISerializeRDF]
		public override XElement ToXElement()
		{
			XElement result = EntityInfo.ToXElement();
            if (IsType(EntityType.PersonName) || IsType(EntityType.PersonInfo))
            {
                string postObjectText = ((PersonEntityInfoBase)EntityInfo).GetPostObject(Children);
                if (!string.IsNullOrEmpty(postObjectText))
                    result.SetAttributeValue(RDF.Attribute.PostObject, postObjectText);
            }
			AddIdAttribute( result );
			result.SetAttributeValue(RDF.Attribute.Value, Value);
			result.SetAttributeValue(RDF.Attribute.StartPosition, _positionInfo.Start);
			if (Role != EntityRole.Undefined)
				result.SetAttributeValue(RDF.Attribute.Role, Role);
            #region [.Тональность.]
            if ( Tonality.IsInquiry )
            {
				result.SetAttributeValue(RDF.Attribute.Type, RDF.Attribute.TYPE_VALUE_INQUIRY);
				result.SetAttributeValue(RDF.Attribute.TYPE_SAVED, Type.GetFullType());

                if ( Tonality.IsInquiryInitial )
                {
					result.SetAttributeValue(RDF.Attribute.INQUIRYINITIAL, bool.TrueString.ToUpper());
                }
            }
            else
            {
				result.SetAttributeValue(RDF.Attribute.Type, Type.GetFullType());
            }
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
            #region [.Анафория.]
            if ( Anafor.AnaforLinkType != AnaforLinkTypeEnum.__UNDEFINE__ )
            {
                throw new NotImplementedException();
                //result.SetAttributeValue( Anafor.AnaforLinkType.ToString(), ... );
            }
            #endregion
            AddChildren(result);
			return result;
		}

		/// <summary>
		/// Добавление дочерних сущностей к заданному элементу
		/// </summary>
		/// <param name="result">элемент</param>
		private void AddChildren(XElement result)
		{
			int position = 0;
			foreach (var child in Children)
			{
				int end = child.PositionInfo.Start - PositionInfo.Start;
				if (end != 0)
					AddText(result, position, end);
				result.Add(child.ToXElement());
				position = child.PositionInfo.End - PositionInfo.Start;
			}
			if (position < Text.Length)
				AddText(result, position, Text.Length);
		}

		/// <summary>
		/// Добавление текста к заданному элементу
		/// </summary>
		/// <param name="result">элемент</param>
		/// <param name="start">начало текста</param>
		/// <param name="end">конец текста</param>
		private void AddText(XElement result, int start, int end)
		{
			result.Add(new UnmarkedText(Text.Substring(start, end - start), start).ToXElement());
		}

		/// <summary>
		/// Добавление идентификатора к заданному элементу, если это необходимо
		/// </summary>
		/// <param name="element">элемент</param>
		private void AddIdAttribute(XElement element)
		{
			int id;
			if (IdGenerator.GetExistedID(this, out id))
				element.SetAttributeValue(RDF.Attribute.ID, id);
		}
		#endregion

        #region [.Тональность.]
        private TonalityInfoEntity _Tonality;
        public TonalityInfoEntity Tonality
        {
            get
            {
                if ( _Tonality == null )
                {
                    _Tonality = new TonalityInfoEntity();
                }
                return (_Tonality);
            }
        }

        /*public void MarkAsInquiryInitial()
        {
            //Tonality.IsInquiry = true;
            Tonality.IsInquiryInitial = true;
        }*/
        public bool IsInquiry
        {
            get { return (Tonality.IsInquiry); }
        }
        public bool IsInquiryInitial
        {
            get { return (Tonality.IsInquiryInitial); }
        }
        public bool HasSNT
        {
            get { return (Tonality.SNT != TonalityType.__UNDEFINE__); }
        }
        public bool HasSNTFINAL
        {
            get { return (Tonality.SNTFINAL != TonalityType.__UNDEFINE__); }
        }
        #endregion

        #region [.Анафория.]
        private AnaforInfo _Anafor;
        public AnaforInfo Anafor
        {
            get
            {
                if (_Anafor == null)
                {
                    _Anafor = new AnaforInfo();
                }
                return (_Anafor);
            }
        }

        public void SetAnafor(  )
        {
            throw new NotImplementedException();
        }
        #endregion
    }


	#region [.Анафория.]
    public enum AnaforLinkTypeEnum
    {
        __UNDEFINE__ = 0,

        LINK             = 1,
        P_LINK           = 2,
        P_LINK_SUBSENT   = 3,
        D_LINK           = 4,
        D_LINK_SUBSENT   = 5,
        CAT_LINK         = 6,
        CAT_LINK_SUBSENT = 7,
        AUT_LINK         = 8,
        SPEECH_LINK      = 9,
    }

    /*public interface IAnafor
    {
        AnaforLinkTypeEnum AnaforLinkType { get; set; }

        Entity  Entity  { get; set; }
        //-OR-
		SubSentence Subsent { get; set; }
    }*/

    [Serializable]
    public class AnaforInfo
    {
        public AnaforLinkTypeEnum AnaforLinkType
        {
            get;
            set;
        }

        public Entity Entity
        {
            get;
            set;
        }
        //-OR-
        public SubSentence Subsent
        {
            get;
            set;
        }
    }
	#endregion


}
