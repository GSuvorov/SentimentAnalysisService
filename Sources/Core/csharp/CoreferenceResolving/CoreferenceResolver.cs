using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Text;
using System.Xml.Linq;
using System.Xml.XPath;

using Linguistics.Core;
using Linguistics.Coreference;

namespace CoreferenceResolving
{
    /// <summary>
    /// 
    /// </summary>
    public sealed class CoreferenceResult : ICoreferenceInfo
    {
        internal CoreferenceResult( XElement rdf, IList< Synset > synsets )
        {
            Rdf     = rdf;
            Synsets = synsets;

            #region [.fill XElement-SynsetName dictionary.]
            _SynsetNameDictionary = new Dictionary< XElement, Tuple< string, XElement > >();
            var n = 0;
            foreach ( var synset in Synsets )
            {
                var tuple = Tuple.Create( synset.SynsetName, synset.MasterEntity );
                foreach ( var xe in synset.Entities )
                {
                    if ( _SynsetNameDictionary.ContainsKey( xe ) )
                    {
                        Debug.WriteLine( (++n) + "). coreference item already exists in result: \r\n" + xe.ToString() );
                    }
                    else
                    {
                        _SynsetNameDictionary.Add( xe, tuple );
                    }
                }
            }
            #endregion
        }

        public XElement Rdf            { get; private set; }
        public IList< Synset > Synsets { get; private set; }        

        #region [.ICoreferenceInfo.]
        private Dictionary< XElement, Tuple< string, XElement > > _SynsetNameDictionary { get; set; }

        public Tuple< string, XElement > TryGetMaster( XElement entity )
        {
            var tuple = default(Tuple< string, XElement >);
            if ( _SynsetNameDictionary.TryGetValue( entity, out tuple ) )
            {
                return (tuple);
            }
            return (null);

            /*
            var synsetName = SynsetNameRetriever.TryRetrieve( entity );
            return (synsetName);
            */
        }
        public string TryGetMasterName( XElement entity )
        {
            var tuple = TryGetMaster( entity );
            return ((tuple != null) ? tuple.Item1 : null);
        }
        #endregion
    }

    /// <summary>
    /// 
    /// </summary>
    public sealed class Synset
    {
        internal Synset( IEnumerable< essence > essences, CoreferenceGroupType groupType )
        {            
            if ( essences == null ) throw (new ArgumentNullException("essences"));
            if ( groupType == CoreferenceGroupType.All ) throw (new ArgumentException("groupType is undefine"));

            var essences_array = essences.ToArray();
            Entities = essences_array.Select( e => e.Entity ).ToArray();
            if ( !Entities.Any() ) throw (new ArgumentException("sequence of essences is empty"));

            MasterEntity = Entities[ 0 ];
            GroupType    = groupType;
            SynsetName   = SynsetDominantNameRetriever.Retrieve( essences_array );
        }

        public XElement[]    Entities         { get; private set; }
        public XElement      MasterEntity     { get; private set; }        
        public CoreferenceGroupType GroupType { get; private set; }
        public string        SynsetName       { get; private set; }

        public override string ToString()
        {
            return ("[GroupType]: "  + GroupType.ToString() +
                  ", [SynsetName]: " + SynsetName.InSingleQuote() +
                  ", [Entities]: "   + Entities.Length +
                  ", [MainEntity]: " + MasterEntity.ToString()
                   );
        }
    }

    /// <summary>
    /// 
    /// </summary>
    public sealed class CoreferenceResolver : ICoreferenceResolver
    {
        #region [.ICoreferenceResolver.]
        public ICoreferenceInfo Process( string   rdf, int rdfPartNumber = 0, CoreferenceGroupType groupType = CoreferenceGroupType.All )
        {
            var xdoc = XDocument.Parse( rdf, LoadOptions.None );
            return (Process( xdoc.Root, rdfPartNumber, groupType ));
        }
        public ICoreferenceInfo Process( XElement rdf, int rdfPartNumber = 0, CoreferenceGroupType groupType = CoreferenceGroupType.All )
        {
            #region [.-1- create essences_groups.]

            var essences_groups = from entity in rdf.DescendantsEntity()
                                    let subsent = entity.GetAncestorSubsent()
                                    where IsGoodSubsent( subsent )

                                    let essence = TryGetEssence( entity, groupType )
                                    where ( essence != null )
                                   
                                  group essence by essence.GroupType;

            var listOfSynset = new List< Synset >();
            var comparer = default(IUniComparer< essence >);
            foreach ( var essences_group in essences_groups )
            {
                var group_type = essences_group.Key;

                comparer = GetIUniComparer( group_type );

                var dummy = default(int);
                var synsets = ProcessGroup( essences_group, comparer, group_type, out dummy );
                listOfSynset.AddRange( synsets );
            }
            #endregion

            #region commented
            /*
            #region [.-2.1- remove synset with only-one items.]
            for ( var i = listOfSynset.Count - 1; 0 <= i; i--  )
            {
                var synset = listOfSynset[ i ];
                if ( synset.Entities.Length <= 1 )
                {
                    listOfSynset.RemoveAt( i );
                }
            }
            #endregion
            */
            #endregion

            #region [.-2.2- set attributes 'COREF-MAIN' & 'COREF-SLAVE' values.]
            var synsetNum = 0;
            foreach ( var synset in listOfSynset )
            {
                //if ( 1 < synset.Entities.Length )
                //{
#if DEBUG
                var coref_id = synset.GroupType + "-" + rdfPartNumber + "-" + synsetNum;
#else
                var coref_id = rdfPartNumber + "-" + synsetNum;
#endif
                synset.MasterEntity.SetAttributeValue( Consts.COREF_MASTER_ATTRIBUTE, coref_id  );

                foreach ( var entity in synset.Entities )
                {
                    if ( entity != synset.MasterEntity )
                    {
                        entity.SetAttributeValue( Consts.COREF_SLAVE_ATTRIBUTE, coref_id );
                    }
                }

                synsetNum++;
                //}
            }
            #endregion

            #region [.-3- return result.]
            var result = new CoreferenceResult( rdf, listOfSynset );
            return (result);
            #endregion
        }

        public ICoreferenceInfo ReadFromRdf( XElement rdf, int rdfPartNumber = 0, bool processIfEmpty = true )
        {
            var mas = from entity in rdf.DescendantsEntity()
                         let coref_master_value = entity.GetAttribute( Consts.COREF_MASTER_ATTRIBUTE )
                         where (coref_master_value != null)
                      select new { 
                                 entity             = entity, 
                                 coref_master_value = coref_master_value 
                             };

            var main_essence = new essence[ 1 ];
            var synsets = new List< Synset >();
            foreach ( var ma in mas )
            {
                var essences = from entity in rdf.DescendantsEntity()
                                 where ( entity.GetAttribute( Consts.COREF_SLAVE_ATTRIBUTE ) == ma.coref_master_value )
                               select 
                                 new essence( entity, entity.GetAttributeTYPE2() );

                main_essence[ 0 ] = new essence( ma.entity, ma.entity.GetAttributeTYPE2() );

                var comparer = GetIUniComparer( main_essence[ 0 ].GroupType );
                var ordered_essences = essences.Concat( main_essence ).OrderBy( e => e, comparer );

                var synset = new Synset( ordered_essences, main_essence[ 0 ].GroupType );

                synsets.Add( synset );
            }

            if ( processIfEmpty && !synsets.Any() && !mas.Any() )
            {
                return (Process( rdf, rdfPartNumber ));
            }

            var result = new CoreferenceResult( rdf, synsets );
            return (result);
        }
        #endregion

        #region [.private method's.]
        private static bool IsGoodSubsent( XElement subsent )
        {
            if ( subsent != null )
            {
                var st = subsent.GetAttributeSUBSENTTYPE();
                switch ( st )
                {
                    case SubSentenceType.Default:
                    case SubSentenceType.ApproximatingForceps:
                    case SubSentenceType.Subordinate:       
                    case SubSentenceType.AdjunctPhrase:
                    case SubSentenceType.Participial:
                    case SubSentenceType.DanglingParticiple:
                        return (true);
                }
            }
            return (false);
        }
        private static essence TryGetEssence( XElement entity, CoreferenceGroupType groupType )
        {
            if ( !entity.HasAttributeNOTTHEME() )
            {
                var type            = entity.GetAttributeTYPE2();
                var entityGroupType = type.ToGroupType();
                if ( entityGroupType.HasValue )
                {
                    if ( HasGoodAncestorsEntity( entity ) )
                    {
                        if ( ((groupType & entityGroupType.Value) == entityGroupType.Value) ||
                              (groupType == CoreferenceGroupType.All) 
                           )
                        {
                            return (new essence( entity, type, entityGroupType.Value ));
                        }
                    }
                }
            }
            return (null);
        }
        private static bool HasGoodAncestorsEntity( XElement entity )
        {
            foreach ( var a in entity.AncestorsEntity() )
            {
                var type = a.GetAttributeTYPE2();
                if ( type.ToGroupType().HasValue )
                {
                    return (false);
                }
                #region
                /*
                switch ( type )
                {
                    case TypeAttributeValue.PersonName:
                    case TypeAttributeValue.PersonInfo:
                    case TypeAttributeValue.PersonNoInfo:
                    case TypeAttributeValue.OrgName:
                    case TypeAttributeValue.OrgAbbr:
                    case TypeAttributeValue.OrgNoInfo:
                    case TypeAttributeValue.PlaceName:
                    case TypeAttributeValue.PlaceNoInfo:
                    case TypeAttributeValue.PseudoGeo:
                    case TypeAttributeValue.Event:
                    case TypeAttributeValue.Product:
                        return (false);
                }
                */
                #endregion
            }
            return (true);
        }

        private static IEnumerable< Synset > ProcessGroup( IEnumerable< essence > essences,
                                                           IUniComparer< essence > comparer, 
                                                           CoreferenceGroupType groupType,
                                                           out int dummy )
        {
            #region [.preliminary pass.]
            dummy = 0;
            foreach ( var x in essences )
            {
                foreach ( var y in essences )
                {
                    if ( object.ReferenceEquals( x, y ) )
                    {
                        dummy++;
                        continue;
                    }

                    if ( comparer.Equals( x, y ) )
                    {
                        dummy++;
                        continue;
                    }
                }
            }
            #endregion

            #region [.main pass.]
            var synsets_groups = from synset_groups 
                                     in essences.GroupBy( e => e, comparer )
       
                                     let ordered_synset_group = synset_groups
                                                .OrderBy( e => e, comparer )
                                     let synset = new Synset( ordered_synset_group, groupType )
                                     where ( 1 < synset.Entities.Length )
                                 select
                                    synset;
            #endregion

            return (synsets_groups);
        }

        private static IUniComparer< essence > GetIUniComparer( CoreferenceGroupType groupType )
        {
            switch ( groupType )
            {
                case CoreferenceGroupType.PersonNameGroup:
                    return (PersonNameGroupEssenceComparer.Instatnce);

                case CoreferenceGroupType.PersonInfoGroup:
                    return (PersonInfoGroupEssenceComparer.Instatnce);

                case CoreferenceGroupType.PersonNoInfoGroup:
                    return (PersonNoInfoGroupEssenceComparer.Instatnce);

                case CoreferenceGroupType.OrgGroup:
                    return (OrgGroupEssenceComparer.Instatnce);

                case CoreferenceGroupType.PlaceGroup:
                    return (PlaceGroupEssenceComparer.Instatnce);

                case CoreferenceGroupType.OrgAbbrGroup:
                    return (OrgAbbrGroupEssenceComparer.Instatnce);

                case CoreferenceGroupType.EventGroup:
                    return (EventGroupEssenceComparer.Instatnce);

                case CoreferenceGroupType.ProductGroup:
                    return (ProductGroupEssenceComparer.Instatnce);

                default:
                    throw (new InvalidOperationException(groupType.ToString()));
            }
        }
        #endregion
    }

    /// <summary>
    /// 
    /// </summary>
    internal sealed class essence
    {
        /// <summary>
        /// 
        /// </summary>
        [Flags]
        internal enum ExistsAttributesEnum : byte
        {
            __NO_ATTRIBUTES__ = 0x0,

            FN    = 1,
            MN    = (1 << 1),
            LN    = (1 << 2),
            VALUE = (1 << 3),
        }

        /// <summary>
        /// 
        /// </summary>
        [Flags]
        internal enum InitialsEnum : byte
        {
            __NO_INITIALS__ = 0x0,

            FN = 1,
            MN = (1 << 1),
        }
     
        private HashSet< essence > _EqualsEssence;

        public essence( XElement entity )
        {            
            Entity     = entity;
            EntityType = entity.GetAttributeTYPE2();
            GroupType  = EntityType.ToGroupType2();
        }
        public essence( XElement entity, EntityType type, CoreferenceGroupType? groupType = null )
        {
            if ( groupType.HasValue && groupType.Value == CoreferenceGroupType.All )
                throw (new ArgumentException(groupType.Value.ToString()));

            Entity     = entity;
            EntityType = type;
            GroupType  = groupType.GetValueOrDefault( type.ToGroupType2() );
        }

        public XElement      Entity     { get; private set; }
        public EntityType    EntityType { get; private set; }
        public CoreferenceGroupType GroupType  { get; private set; }

        public void AddEqualEssence( essence item )
        {
            if ( _EqualsEssence == null )
            {
                _EqualsEssence = new HashSet< essence >();
            }            
#if DEBUG
            var r = _EqualsEssence.Add( item );
            if ( r && (item._EqualsEssence == null || !item._EqualsEssence.Contains( this )) )
            {
                System.Diagnostics.Debug.WriteLine("merging:\r\n  1. '" + this + "'\r\n  2. '" + item + '\'' + Environment.NewLine);
            }
#else
            _EqualsEssence.Add( item );
#endif
        }
        public IEnumerable< essence > EqualsEssence
        {
            get 
            {
                if ( _EqualsEssence == null )
                {
                    throw (new InvalidOperationException("_EqualsEssence == null"));
                    //---return (Enumerable.Empty< essence >());
                }
                return (_EqualsEssence); 
            }    
        }
        public bool HasEqualsEssence
        {
            get { return (_EqualsEssence != null); }
        }
        public bool HasOverlapsByEqualEssence( essence item )
        {
            if ( _EqualsEssence == null || item._EqualsEssence == null)
                return (false);

            return (_EqualsEssence.Overlaps( item._EqualsEssence ));
        }

        public override string ToString()
        {
            return (Entity.ToString());
        }

        private string _VALUE;
        public string VALUE
        {
            get 
            {
                if ( _VALUE == null )
                {
                    _VALUE = Entity.GetAttributeVALUE();
                    if (_VALUE == null)
                        _VALUE = string.Empty;
                }
                return (_VALUE);
            }
        }

        private string _VALUE_THEME;
        public string VALUE_THEME 
        {
            get
            {
                if (_VALUE_THEME == null)
                {
                    _VALUE_THEME = Entity.GetAttributeVALUETHEME();
                    if (_VALUE_THEME == null)
                        _VALUE_THEME = string.Empty;
                }
                return (_VALUE_THEME);
            }
        }

        private string _LN;
        public string LN
        {
            get 
            {
                if (_LN == null)
                {
                    _LN = Entity.GetAttributeLN();
                    if (_LN == null)
                        _LN = string.Empty;
                }
                return (_LN);
            }
        }

        private string _FN;
        public string FN
        {
            get
            {
                if (_FN == null)
                {
                    _FN = Entity.GetAttributeFN();
                    if (_FN == null)
                        _FN = string.Empty;
                }
                return (_FN);
            }
        }

        private string _MN;
        public string MN
        {
            get
            {
                if (_MN == null)
                {
                    _MN = Entity.GetAttributeMN();
                    if (_MN == null)
                        _MN = string.Empty;
                }
                return (_MN);
            }
        }

        private string _FN_MN;
        public string FN_MN
        {
            get
            {
                if (_FN_MN == null)
                {
                    if ( !FN.IsEmptyOrNull() && !MN.IsEmptyOrNull() )
                    {
                        _FN_MN = FN + ' ' + MN;
                    }
                    else
                    {
                        _FN_MN = string.Empty;
                    }
                }
                return (_FN_MN);
            }
        }

        private string _LN_FN;
        public string LN_FN
        {
            get
            {
                if (_LN_FN == null)
                {
                    if ( !FN.IsEmptyOrNull() && !LN.IsEmptyOrNull() )
                    {
                        _LN_FN = FN + ' ' + LN;
                    }
                    else
                    {
                        _LN_FN = string.Empty;
                    }
                }
                return (_LN_FN);
            }
        }

        private string _FN_LN;
        public string FN_LN
        {
            get
            {
                if (_FN_LN == null)
                {
                    if ( !FN.IsEmptyOrNull() && !LN.IsEmptyOrNull() )
                    {
                        _FN_LN = FN + ' ' + LN;
                    }
                    else
                    {
                        _FN_LN = string.Empty;
                    }
                }
                return (_FN_LN);
            }
        }

        private string _LN_FN_MN;
        public string LN_FN_MN
        {
            get
            {
                if (_LN_FN_MN == null)
                {
                    if ( !LN.IsEmptyOrNull() && !FN_MN.IsEmptyOrNull() )
                    {
                        _LN_FN_MN = LN + ' ' + FN_MN;
                    }
                    else
                    {
                        _LN_FN_MN = string.Empty;
                    }
                }
                return (_LN_FN_MN);
            }
        }

        private string _FN_MN_LN;
        public string FN_MN_LN
        {
            get
            {
                if (_FN_MN_LN == null)
                {
                    if ( !LN.IsEmptyOrNull() && !FN_MN.IsEmptyOrNull() )
                    {
                        _FN_MN_LN = FN_MN + ' ' + LN;
                    }
                    else
                    {
                        _FN_MN_LN = string.Empty;
                    }
                }
                return (_FN_MN_LN);
            }
        }

        private string _PRE;
        public string PRE
        {
            get 
            {
                if (_PRE == null)
                {
                    _PRE = Entity.GetAttributePRE();
                    if (_PRE == null)
                        _PRE = string.Empty;
                }
                return (_PRE);
            }
        }

        private string _PROF;
        public string PROF
        {
            get
            {
                if (_PROF == null)
                {
                    _PROF = Entity.GetAttributePROF();
                    if (_PROF == null)
                        _PROF = string.Empty;
                }
                return (_PROF);
            }
        }

        private string _PRE_PROF;
        public string PRE_PROF
        {
            get
            {
                if (_PRE_PROF == null)
                {
                    if ( !PRE.IsEmptyOrNull() && !PROF.IsEmptyOrNull() )
                    {
                        _PRE_PROF = PRE + ' ' + PROF;
                    }
                    else
                    {
                        _PRE_PROF = string.Empty;
                    }                    
                }
                return (_PRE_PROF);
            }
        }

        private string _NAME;
        public string NAME
        {
            get
            {
                if (_NAME == null)
                {
                    _NAME = Entity.GetAttributeNAME();
                    if (_NAME == null)
                        _NAME = string.Empty;
                }
                return (_NAME);
            }
        }

        private string _PROF_LN;
        public string PROF_LN
        {
            get
            {
                if (_PROF_LN == null)
                {
                    if ( !PROF.IsEmptyOrNull() && !LN.IsEmptyOrNull() )
                    {
                        _PROF_LN = PROF + ' ' + LN;
                    }
                    else
                    {
                        _PROF_LN = string.Empty;
                    }
                }
                return (_PROF_LN);
            }
        }

        private string _POST;
        public string POST
        {
            get
            {
                if (_POST == null)
                {
                    _POST = Entity.GetAttributePOST();
                    if (_POST == null)
                        _POST = string.Empty;
                }
                return (_POST);
            }
        }

        private string _PO;
        public string PO
        {
            get
            {
                if (_PO == null)
                {
                    _PO = Entity.GetAttributePO();
                    if (_PO == null)
                        _PO = string.Empty;
                }
                return (_PO);
            }
        }

        private ExistsAttributesEnum? _ExistsAttributes;
        public ExistsAttributesEnum ExistsAttributes
        {
            get
            {
                if ( !_ExistsAttributes.HasValue )
                {
                    _ExistsAttributes = ExistsAttributesEnum.__NO_ATTRIBUTES__;
                    if ( !this.FN   .IsEmptyOrNull() ) _ExistsAttributes |= ExistsAttributesEnum.FN;
                    if ( !this.LN   .IsEmptyOrNull() ) _ExistsAttributes |= ExistsAttributesEnum.LN;
                    if ( !this.MN   .IsEmptyOrNull() ) _ExistsAttributes |= ExistsAttributesEnum.MN;
                    if ( !this.VALUE.IsEmptyOrNull() ) _ExistsAttributes |= ExistsAttributesEnum.VALUE;
                }
                return (_ExistsAttributes.Value);
            }
        }

        private InitialsEnum? _Initials;
        public InitialsEnum Initials
        {
            get
            {
                if ( !_Initials.HasValue )
                {
                    _Initials = InitialsEnum.__NO_INITIALS__;
                    if ( is_Initials( this.FN ) ) _Initials |= InitialsEnum.FN;
                    if ( is_Initials( this.MN ) ) _Initials |= InitialsEnum.MN;
                }
                return (_Initials.Value);
            }
        }

        private static bool is_Initials( string value )
        {
            if ( !string.IsNullOrEmpty( value ) ) 
            {
                if ( (2 <= value.Length && value.Length <= 4) && 
                     char.IsUpper(value[ 0 ]) && (value[ value.Length - 1 ] == '.') )
                {
                    return (true);
                }
            }
            return (false);
        }

        public int Tag
        {
            get;
            set;
        }
    }

    /*/// <summary>
    /// 
    /// </summary>
    internal class ListOfEssenceLists : List< List< essence > >
    {
        private readonly IEqualityComparer< essence > _Comparer;

        public ListOfEssenceLists( IEqualityComparer< essence > comparer )
        {
            _Comparer = comparer;
        }

        public void AddEssence( essence item )
        {
            for ( int i = 0, len = Count; i < len; i++ )
            {
                var lst = this[ i ];
                for ( int j = 0, len2 = lst.Count; j < len2; j++ )
                {
                    if ( _Comparer.Equals( lst[ j ], item ) )
                    {
                        lst.Add( item );
                        return;
                    }
                }
            }

            var lst_new = new List< essence >();
            lst_new.Add( item );
            this.Add( lst_new );
        }
    }*/

    /// <summary>
    /// 
    /// </summary>
    internal interface IUniComparer< T > : IEqualityComparer< T >, IComparer< T > { }
    
    /// <summary>
    /// 
    /// </summary>
    internal abstract class GroupEssenceComparerBase : IUniComparer< essence >
    {
        #region [.IEqualityComparer< essence >.]
        public bool Equals( essence x, essence y )
        {
            if ( EqualsInternal( x, y ) )
            {
                return (true);
            }

            return (CompareByEqualsEssences( x, y ));
        }
        public int GetHashCode( essence obj )
        {
            return (0);
            //return (obj.GroupType.GetHashCode());
        }

        protected abstract bool EqualsInternal( essence x, essence y );

        protected bool CompareByEqualsEssences( essence x, essence y )
        {
            if ( x.HasOverlapsByEqualEssence( y ) )
                return (AddMutualToEqualsReturnTrue( x, y ));

            if ( x.HasEqualsEssence )
                foreach ( var e in x.EqualsEssence )
                {
                    if ( EqualsInternal( e, y ) )
                        return (AddMutualToEqualsReturnTrue( x, y ));
                }

            if ( y.HasEqualsEssence )
                foreach ( var e in y.EqualsEssence )
                {
                    if ( EqualsInternal( x, e ) )
                        return (AddMutualToEqualsReturnTrue( x, y ));
                }

            return (false);
        }
        protected static bool AddMutualToEqualsReturnTrue( essence x, essence y )
        {
            x.AddEqualEssence( y );
            y.AddEqualEssence( x );

            return (true);
        }
        /*protected static bool IsAncestorsDescendants( essence x, essence y )
        {
            if ( x.Entity.Ancestors( Consts.ENTITY_ELEMENT ).Any( a => a == y.Entity ) ||
                 y.Entity.Ancestors( Consts.ENTITY_ELEMENT ).Any( a => a == x.Entity ) 
               )
            {
                return (true);
            }

            return (false);
        }*/

        protected static bool EQUAL( string x, string y )
        {
            return ((string.Compare( x, y, true ) == 0) && !string.IsNullOrEmpty( x ));
        }
        #endregion

        #region [.IComparer< essence >.]
        public abstract int Compare( essence x, essence y );

        protected static int getlen_PRE_PROF( essence e )
        {
            if ( !e.PRE_PROF.IsEmptyOrNull() )
                return (e.PRE_PROF.Length);
            return (0);
        }
        protected static int getlen_PROF( essence e )
        {
            if ( !e.PROF.IsEmptyOrNull() )
                return (e.PROF.Length);
            return (0);
        }
        protected static int getlen_VALUE( essence e )
        {
            if ( !e.VALUE.IsEmptyOrNull() )
                return (e.VALUE.Length);
            return (0);
        }
        #endregion

        public abstract string GetDominantName( essence[] essences );
    }

    /// <summary>
    /// 
    /// </summary>
    internal sealed class PersonNameGroupEssenceComparer : GroupEssenceComparerBase
    {
        #region [.IEqualityComparer< TypeAttributeValue >.]
        public static readonly PersonNameGroupEssenceComparer Instatnce = new PersonNameGroupEssenceComparer();
        private PersonNameGroupEssenceComparer() { }

        protected override bool EqualsInternal( essence x, essence y )
        {
            var d = default(bool?);

            #region [a) .LN + FN + MN = LN + FN + MN.]
            if ( x.ExistsAttributes.has_FN_and_LN_and_MN() && 
                 y.ExistsAttributes.has_FN_and_LN_and_MN() 
               )
            {
                d = EQUAL_FN_MN_asInitials( x, y );
                if ( d.GetValueOrDefault( false ) )
                {
                    d = EQUAL_ifNonEmpty( x.LN, y.LN );
                }
                else
                {
                    d = EQUAL_ifNonEmpty( x.LN_FN_MN, y.LN_FN_MN );
                }
                
                if ( d.HasValue )
                {
                    if ( d.Value )
                    {
                        x.Tag++; y.Tag++;
                        return (AddMutualToEqualsReturnTrue( x, y ));
                    }
                    return (false);
                }
            }
            #endregion

            #region [b) .FN + MN (или тут нет LN) = FN + MN (или тут нет LN).]
            if ( !x.ExistsAttributes.has_LN() || 
                 !y.ExistsAttributes.has_LN() 
               )
            {
                d = EQUAL_FN_MN_asInitials( x, y );
                if ( !d.HasValue )
                {
                    d = EQUAL_ifNonEmpty( x.FN_MN, y.FN_MN );
                }

                if ( d.HasValue )
                {
                    if ( d.Value )
                    {
                        x.Tag++; y.Tag++;
                        return (AddMutualToEqualsReturnTrue( x, y ));
                    }
                    return (false);
                }
            }
            #endregion

            #region [c) .LN + FN (или тут нет MN) = LN + FN (или тут нет MN).]
            if ( !x.ExistsAttributes.has_MN() || 
                 !y.ExistsAttributes.has_MN() 
               )
            {
                d = EQUAL_FN_asInitials( x, y );
                if ( d.GetValueOrDefault( false ) )
                {
                    d = EQUAL_ifNonEmpty( x.LN, y.LN );
                }
                else
                {
                    d = EQUAL_ifNonEmpty( x.LN_FN, y.LN_FN );
                }

                if ( d.HasValue )
                {
                    if ( d.Value )
                    {
                        x.Tag++; y.Tag++;
                        return (AddMutualToEqualsReturnTrue( x, y ));
                    }
                    return (false);
                }
            }
            #endregion

            #region [d) //---same-a-point-(e)---// .LN + FN + MN = LN (нет FN, MN).]
            /* 
            if ( (x.ExistsAttributes.has_FN_and_LN_and_MN() && 
                  y.ExistsAttributes.has_LN()               &&
                  y.ExistsAttributes.hasNot_FN_and_MN())
                    ||
                 (y.ExistsAttributes.has_FN_and_LN_and_MN() && 
                  x.ExistsAttributes.has_LN()               &&
                  x.ExistsAttributes.hasNot_FN_and_MN())
               )
            {
                d = EQUAL_ifNonEmpty( x.LN, y.LN );
                if ( d.HasValue )
                {
                    if ( d.Value )
                        return (AddMutualToEqualsReturnTrue( x, y ));
                    return (false);
                }
            }*/
            #endregion
            #region [e) .LN + FN = LN (нет FN, MN).]
            if ( (x.ExistsAttributes.has_FN_and_LN()    && 
                  y.ExistsAttributes.has_LN()           &&
                  y.ExistsAttributes.hasNot_FN_and_MN() &&
                 !y.HasEqualsEssence )
                    ||
                 (y.ExistsAttributes.has_FN_and_LN()    && 
                  x.ExistsAttributes.has_LN()           &&
                  x.ExistsAttributes.hasNot_FN_and_MN() &&
                 !x.HasEqualsEssence )
               )
            {
                d = EQUAL_ifNonEmpty( x.LN, y.LN );
                if ( d.HasValue )
                {
                    if ( d.Value )
                        return (AddMutualToEqualsReturnTrue( x, y ));
                    return (false);
                }
            }
            #endregion

            #region [f) .LN + FN + MN = FN (нет LN, MN).]
            if ( (x.ExistsAttributes.has_FN_and_LN_and_MN() && 
                  y.ExistsAttributes.has_FN()               &&
                  y.ExistsAttributes.hasNot_LN_and_MN()     &&
                 !y.HasEqualsEssence )
                    ||
                 (y.ExistsAttributes.has_FN_and_LN_and_MN() && 
                  x.ExistsAttributes.has_FN()               &&
                  x.ExistsAttributes.hasNot_LN_and_MN()     &&
                 !x.HasEqualsEssence )
               )
            {
                d = EQUAL_ifNonEmpty( x.FN, y.FN );
                if ( d.HasValue )
                {
                    if ( d.Value )
                        return (AddMutualToEqualsReturnTrue( x, y ));
                    return (false);
                }
            }
            #endregion

            #region [???) .FN + LN (нет MN) = FN + MN (нет LN).]
            if ( (x.ExistsAttributes.has_FN_and_LN()  &&
                 !x.ExistsAttributes.has_MN()         && 
                  y.ExistsAttributes.has_FN_and_MN()  &&
                 !y.ExistsAttributes.has_LN()         &&
                 !y.HasEqualsEssence)
                    ||
                 (y.ExistsAttributes.has_FN_and_LN()  &&
                 !y.ExistsAttributes.has_MN()         && 
                  x.ExistsAttributes.has_FN_and_MN()  &&
                 !x.ExistsAttributes.has_LN()         &&
                 !x.HasEqualsEssence)
               )
            {
                d = EQUAL_ifNonEmpty( x.FN, y.FN );
                if ( d.HasValue )
                {
                    if ( d.Value )
                        return (AddMutualToEqualsReturnTrue( x, y ));
                    return (false);
                }
            }
            #endregion

            #region [g) .FN + MN = FN (нет LN, MN).]
            if ( (x.ExistsAttributes.has_FN_and_MN()    && 
                  y.ExistsAttributes.has_FN()           &&
                  y.ExistsAttributes.hasNot_LN_and_MN() &&
                 !y.HasEqualsEssence)
                    ||
                 (y.ExistsAttributes.has_FN_and_MN()    && 
                  x.ExistsAttributes.has_FN()           &&
                  x.ExistsAttributes.hasNot_LN_and_MN() &&
                 !x.HasEqualsEssence)
               )
            {
                d = EQUAL_ifNonEmpty( x.FN, y.FN );
                if ( d.HasValue )
                {
                    if ( d.Value )
                        return (AddMutualToEqualsReturnTrue( x, y ));
                    return (false);
                }
            }
            #endregion

            #region [h) .FN + LN (нет MN) = FN (нет LN, MN).]
            if ( (x.ExistsAttributes.has_FN_and_LN()    && 
                 !x.ExistsAttributes.has_MN()           &&
                  y.ExistsAttributes.has_FN()           &&
                  y.ExistsAttributes.hasNot_LN_and_MN() &&
                 !y.HasEqualsEssence)
                    ||
                 (y.ExistsAttributes.has_FN_and_LN()    &&
                 !y.ExistsAttributes.has_MN()           &&
                  x.ExistsAttributes.has_FN()           &&
                  x.ExistsAttributes.hasNot_LN_and_MN() &&
                 !x.HasEqualsEssence)
               )
            {
                d = EQUAL_ifNonEmpty( x.FN, y.FN );
                if ( d.HasValue )
                {
                    if ( d.Value )
                        return (AddMutualToEqualsReturnTrue( x, y ));
                    return (false);
                }
            }
            #endregion

            #region [i) .FN + MN + LN = VALUE (нет FN, LN, MN).]
            if ( y.ExistsAttributes.hasNot_FN_and_LN_and_MN() )
            {
                d = EQUAL_ifNonEmpty( x.FN_MN_LN, y.VALUE );
                if ( d.HasValue )
                {
                    if ( d.Value )
                        return (AddMutualToEqualsReturnTrue( x, y ));
                    return (false);
                }
            }
            if ( x.ExistsAttributes.hasNot_FN_and_LN_and_MN() )
            {
                d = EQUAL_ifNonEmpty( y.FN_MN_LN, x.VALUE );
                if ( d.HasValue )
                {
                    if ( d.Value )
                        return (AddMutualToEqualsReturnTrue( x, y ));
                    return (false);
                }
            }
            #endregion

            #region [j) .FN + MN (нет LN) = VALUE (нет FN, LN, MN).]
            if ( !x.ExistsAttributes.has_LN() &&
                  y.ExistsAttributes.hasNot_FN_and_LN_and_MN() 
               )
            {
                d = EQUAL_ifNonEmpty( x.FN_MN, y.VALUE );
                if ( d.HasValue )
                {
                    if ( d.Value )
                        return (AddMutualToEqualsReturnTrue( x, y ));
                    return (false);
                }
            }
            if ( !y.ExistsAttributes.has_LN() && 
                  x.ExistsAttributes.hasNot_FN_and_LN_and_MN() 
               )
            {
                d = EQUAL_ifNonEmpty( y.FN_MN, x.VALUE );
                if ( d.HasValue )
                {
                    if ( d.Value )
                        return (AddMutualToEqualsReturnTrue( x, y ));
                    return (false);
                }
            }
            #endregion

            #region [k) .PROF + LN (или тут нет FN и MN) = PROF + LN (или тут нет FN и MN).]
            if ( x.ExistsAttributes.hasNot_FN_and_MN() || 
                 y.ExistsAttributes.hasNot_FN_and_MN() 
               )
            {
                d = EQUAL_ifNonEmpty( x.PROF_LN, y.PROF_LN );
                if ( d.HasValue )
                {
                    if ( d.Value )
                    {
                        return (AddMutualToEqualsReturnTrue( x, y ));
                    }
                    return (false);
                }
            }
            #endregion

            #region [l) .LN (нет FN, MN) = LN (нет FN, MN).]
            if ( x.ExistsAttributes.hasNot_FN_and_MN() && 
                 y.ExistsAttributes.hasNot_FN_and_MN()
               )
            {
                d = EQUAL_ifNonEmpty( x.LN, y.LN );
                if ( d.HasValue )
                {
                    if ( d.Value )
                        return (AddMutualToEqualsReturnTrue( x, y ));
                    return (false);
                }
            }
            #endregion

            #region [m) .FN (нет LN, MN) = FN (нет LN, MN).]
            if ( x.ExistsAttributes.hasNot_LN_and_MN() && 
                 y.ExistsAttributes.hasNot_LN_and_MN() 
               )
            {
                d = EQUAL_ifNonEmpty( x.FN, y.FN );
                if ( d.HasValue )
                {
                    if ( d.Value )
                        return (AddMutualToEqualsReturnTrue( x, y ));
                    return (false);
                }
            }
            #endregion

            #region [n) .LN (нет FN, MN) = VALUE (нет FN, LN, MN).]
            if ( x.ExistsAttributes.hasNot_FN_and_MN() && 
                 y.ExistsAttributes.hasNot_FN_and_LN_and_MN() 
               )
            {
                d = EQUAL_ifNonEmpty( x.LN, y.VALUE );
                if ( d.HasValue )
                {
                    if ( d.Value )
                        return (AddMutualToEqualsReturnTrue( x, y ));
                    return (false);
                }
            }
            if ( y.ExistsAttributes.hasNot_FN_and_MN() &&
                 x.ExistsAttributes.hasNot_FN_and_LN_and_MN() 
               )
            {
                d = EQUAL_ifNonEmpty( y.LN, x.VALUE );
                if ( d.HasValue )
                {
                    if ( d.Value )
                        return (AddMutualToEqualsReturnTrue( x, y ));
                    return (false);
                }
            }
            #endregion

            #region [o) .VALUE (нет FN, LN, MN) = VALUE (нет FN, LN, MN).]
            if ( x.ExistsAttributes.hasNot_FN_and_LN_and_MN() && 
                 y.ExistsAttributes.hasNot_FN_and_LN_and_MN() 
               )
            {
                d = EQUAL_ifNonEmpty( x.VALUE, y.VALUE );
                if ( d.HasValue )
                {
                    if ( d.Value )
                        return (AddMutualToEqualsReturnTrue( x, y ));
                    return (false);
                }
            }
            #endregion

            return (false);
        }

        private static bool? EQUAL_ifNonEmpty( string x, string y )
        {
            if ( !x.IsEmptyOrNull() && !y.IsEmptyOrNull() )
            {
                return (string.Compare( x, y, true ) == 0);
            }
            return (null);
        }
        private static bool? EQUAL_FN_MN_asInitials( essence x, essence y )
        {
            if ( x.Initials.has_FN_and_MN() && y.ExistsAttributes.has_FN_and_MN() )
            {
                return ((string.Compare(x.FN, 0, y.FN, 0, x.FN.Length - 1) == 0) && 
                        (string.Compare(x.MN, 0, y.FN, 0, x.MN.Length - 1) == 0));
            }
            if ( y.Initials.has_FN_and_MN() && x.ExistsAttributes.has_FN_and_MN() )
            {
                return ((string.Compare(y.FN, 0, x.FN, 0, y.FN.Length - 1) == 0) && 
                        (string.Compare(y.MN, 0, x.FN, 0, y.MN.Length - 1) == 0));
            }
            return (null);
        }
        private static bool? EQUAL_FN_asInitials( essence x, essence y )
        {
            if ( x.Initials.has_FN() && y.ExistsAttributes.has_FN() )
            {
                return (string.Compare(x.FN, 0, y.FN, 0, x.FN.Length - 1) == 0);
            }
            if ( y.Initials.has_FN() && x.ExistsAttributes.has_FN() )
            {
                return (string.Compare(y.FN, 0, x.FN, 0, y.FN.Length - 1) == 0);
            }
            return (null);
        }
        #endregion 
    
        #region [.IComparer< essence >.]
        public override int Compare( essence x, essence y )
        {
            if ( object.ReferenceEquals( x, y ) )
                return (0);

            /*var d = y.Tag - x.Tag;
            if ( d != 0 ) return (d);

            d = x.EntityType - y.EntityType;
            if ( d != 0 ) return (d);*/

            var d = getlen_PROF_LN_FN_MN( y ) - getlen_PROF_LN_FN_MN( x );
            if (d != 0) return (d);

            d = getlen_LN_FN_MN( y ) - getlen_LN_FN_MN( x );
            if (d != 0) return (d);

            d = getlen_FN_MN( y ) - getlen_FN_MN( x );
            if (d != 0) return (d);

            d = getlen_FN_LN( y ) - getlen_FN_LN( x );
            if (d != 0) return (d);

            d = getreallen_FN( y ) - getreallen_FN( x );
            if (d != 0) return (d);
            d = getreallen_MN( y ) - getreallen_MN( x );
            if (d != 0) return (d);

            d = getlen_VALUE( y ) - getlen_VALUE( x );
            if (d != 0) return (d);

            d = getlen_LN( y ) - getlen_LN( x );
            if (d != 0) return (d);

            d = y.ExistsAttributes - x.ExistsAttributes;
            if (d != 0) return (d);

            d = y.Tag - x.Tag;
            if (d != 0) return (d);

            d = x.EntityType - y.EntityType;
            if (d != 0) return (d);

            return (0);
        }

        private static int getlen_PROF_LN_FN_MN( essence e )
        {
            if ( !e.LN_FN_MN.IsEmptyOrNull() && 
                 !e.PROF.IsEmptyOrNull() && char.IsUpper( e.PROF, 0 ) 
               )
            {
                return (e.LN_FN_MN.Length);
            }
            return (0);
        }
        private static int getlen_LN_FN_MN( essence e )
        {
            if ( !e.LN_FN_MN.IsEmptyOrNull() )
                return (e.LN_FN_MN.Length);
            return (0);
        }
        private static int getlen_FN_MN( essence e )
        {
            if ( !e.FN_MN.IsEmptyOrNull() )
                return (e.FN_MN.Length);
            return (0);
        }
        private static int getlen_FN_LN( essence e )
        {
            if ( !e.FN_LN.IsEmptyOrNull() )
                return (e.FN_LN.Length);
            return (0);
        }
        private static int getlen_LN( essence e )
        {
            if ( !e.LN.IsEmptyOrNull() )
                return (e.LN.Length);
            return (0);
        }
        private static int getreallen_FN( essence e )
        {
            if ( !e.FN.IsEmptyOrNull() )
                return (e.FN.Length);
            return (0);
        }
        private static int getreallen_MN( essence e )
        {
            if ( !e.MN.IsEmptyOrNull() )
                return (e.MN.Length);
            return (0);
        }
        #endregion

        /*private string GetBestName( essence e )
        {
            if ( !e.LN_FN_MN.IsEmptyOrNull() && 
                 !e.PROF.IsEmptyOrNull() && char.IsUpper( e.PROF, 0 ) 
               )
            {
                return (e.PROF + ' ' + e.LN_FN_MN);
            }

            if ( !e.LN_FN_MN.IsEmptyOrNull() )
                return (e.LN_FN_MN);

            if ( !e.FN_MN.IsEmptyOrNull() )
                return (e.FN_MN);

            if ( !e.LN_FN.IsEmptyOrNull() )
                return (e.LN_FN);

            if ( !e.VALUE.IsEmptyOrNull() )
                return (e.VALUE);

            return (e.LN);
        }        */
        private static string GetLongestPOST( essence[] essences )
        {
            var POSTvalues = from e in essences 
                                where (!e.POST.IsEmptyOrNull())
                                orderby e.POST.Length descending
                             select e.POST;

            return (POSTvalues.FirstOrDefault());
        }
        private static string GetLongestPO( essence[] essences )
        {
            var POvalues = from e in essences 
                                where (!e.PO.IsEmptyOrNull())
                                orderby e.PO.Length descending
                             select e.PO;

            return (POvalues.FirstOrDefault());
        }
        private static string GetLongestPROF( essence[] essences )
        {
            var PROFvalues = from e in essences 
                                where (!e.PROF.IsEmptyOrNull() && char.IsUpper( e.PROF, 0 ))
                                orderby e.PROF.Length descending
                             select e.PROF;

            return (PROFvalues.FirstOrDefault());
        }
        private static string GetLongestLN( essence[] essences )
        {
            var LNvalues = from e in essences 
                                where (!e.LN.IsEmptyOrNull())
                                orderby e.LN.Length descending
                           select e.LN;

            return (LNvalues.FirstOrDefault());
        }
        private static string GetLongestFN( essence[] essences )
        {
            var FNvalues = from e in essences 
                                where (!e.FN.IsEmptyOrNull())
                                orderby e.FN.Length descending
                           select e.FN;

            return (FNvalues.FirstOrDefault());
        }
        private static string GetLongestMN( essence[] essences )
        {
            var MNvalues = from e in essences 
                                where (!e.MN.IsEmptyOrNull())
                                orderby e.MN.Length descending
                           select e.MN;

            return (MNvalues.FirstOrDefault());
        }
        private static string GetLongestVALUETHEMEorVALUE(essence[] essences)
        {
            var VALUEvalues = from e in essences 
                                let v = !e.VALUE_THEME.IsEmptyOrNull() ? e.VALUE_THEME : e.VALUE
                                where (!v.IsEmptyOrNull())
                                orderby v.Length descending
                              select v;

            return (VALUEvalues.FirstOrDefault());
        }    
        public override string GetDominantName( essence[] essences )
        {
            var FN = GetLongestFN( essences );
            var LN = GetLongestLN( essences );

            if ( !FN.IsEmptyOrNull() )
            {                
                var MN = GetLongestMN( essences );                
                if ( !MN.IsEmptyOrNull() )
                {
                    if ( !LN.IsEmptyOrNull() )
                    {
                        #region
                        var POST = GetLongestPOST( essences );
                        if ( !POST.IsEmptyOrNull() )
                        {
                            var PO = GetLongestPO( essences );
                            if ( !PO.IsEmptyOrNull() )
                                return (POST + ' ' + PO + ' ' + LN + ' ' + FN + ' ' + MN);
                            else
                                return (POST + ' ' + LN + ' ' + FN + ' ' + MN);
                        }
                        else
                        {
                            var PROF = GetLongestPROF( essences );
                            if ( !PROF.IsEmptyOrNull() )
                                return (PROF + ' ' + LN + ' ' + FN + ' ' + MN);
                            else
                                return (FN + ' ' + MN + ' ' + LN);
                        }
                        #endregion
                    }
                    return (FN + ' ' + MN);
                }
                else if ( !LN.IsEmptyOrNull() )
                {
                    #region
                    var POST = GetLongestPOST( essences );
                    if ( !POST.IsEmptyOrNull() )
                    {
                        var PO = GetLongestPO( essences );
                        if ( !PO.IsEmptyOrNull() )
                            return (POST + ' ' + PO + ' ' + LN + ' ' + FN);
                        else
                            return (POST + ' ' + LN + ' ' + FN);
                    }
                    else
                    {
                        var PROF = GetLongestPROF( essences );
                        if ( !PROF.IsEmptyOrNull() )
                            return (PROF + ' ' + LN + ' ' + FN);
                    }
                    #endregion

                    return (LN + ' ' + FN);
                }
            }

            var VALUE = GetLongestVALUETHEMEorVALUE( essences );
            if ( !VALUE.IsEmptyOrNull() )
            {
                return (VALUE);
            }

            return (LN);
        }
    }

    /// <summary>
    /// 
    /// </summary>
    internal class PersonInfoGroupEssenceComparer : GroupEssenceComparerBase
    {
        #region [.IEqualityComparer< TypeAttributeValue >.]
        public static readonly PersonInfoGroupEssenceComparer Instatnce = new PersonInfoGroupEssenceComparer();
        protected PersonInfoGroupEssenceComparer() { }

        protected override bool EqualsInternal( essence x, essence y )
        {
            /*if ( IsAncestorsDescendants( x, y ) )
            {
                return (false);
            }*/

            if ( EQUAL( x.PROF, y.PROF ) )
                return (AddMutualToEqualsReturnTrue( x, y ));

            if ( EQUAL( x.PROF, y.VALUE ) || EQUAL( x.VALUE, y.PROF ) )
                return (AddMutualToEqualsReturnTrue( x, y ));

            if ( EQUAL( x.VALUE, y.VALUE ) )
                return (AddMutualToEqualsReturnTrue( x, y ));

            return (false);
        }
        #endregion 
    
        #region [.IComparer< essence >.]
        public override int Compare( essence x, essence y )
        {
            if ( object.ReferenceEquals( x, y ) )
                return (0);

            var d = getlen_PROF( y ) - getlen_PROF( x );
            if (d != 0) return (d);

            d = getlen_VALUE( y ) - getlen_VALUE( x );
            if (d != 0) return (d);

            return (0);
        }
        #endregion

        private string GetBestName( essence e )
        {
            if ( !e.PROF.IsEmptyOrNull() )
                return (e.PROF);

            return (e.VALUE);
        }
        public override string GetDominantName( essence[] essences )
        {
            return (GetBestName( essences[ 0 ] ));
        }
    }

    /// <summary>
    /// 
    /// </summary>
    internal sealed class PersonNoInfoGroupEssenceComparer : PersonInfoGroupEssenceComparer
    {
        new public static readonly PersonNoInfoGroupEssenceComparer Instatnce = new PersonNoInfoGroupEssenceComparer();
        private PersonNoInfoGroupEssenceComparer() { }
    }

    /// <summary>
    /// 
    /// </summary>
    internal class OrgGroupEssenceComparer : GroupEssenceComparerBase
    {
        #region [.IEqualityComparer< TypeAttributeValue >.]
        public static readonly OrgGroupEssenceComparer Instatnce = new OrgGroupEssenceComparer();
        protected OrgGroupEssenceComparer() { }

        protected override bool EqualsInternal( essence x, essence y )
        {
            /*if ( IsAncestorsDescendants( x, y ) )
            {
                return (false);
            }*/

            if ( EQUAL( x.VALUE, y.VALUE ) )
                return (AddMutualToEqualsReturnTrue( x, y ));

            if ( EQUAL( x.NAME, y.NAME ) )
                return (AddMutualToEqualsReturnTrue( x, y ));

            if ( EQUAL( x.PRE_PROF, y.PRE_PROF ) )
                return (AddMutualToEqualsReturnTrue( x, y ));

            if ( EQUAL( x.NAME, y.VALUE ) || EQUAL( x.VALUE, y.NAME ) )
                return (AddMutualToEqualsReturnTrue( x, y ));

            if ( EQUAL( x.NAME, y.PRE_PROF ) || EQUAL( x.PRE_PROF, y.NAME ) )
                return (AddMutualToEqualsReturnTrue( x, y ));

            if ( EQUAL( x.PROF, y.VALUE ) || EQUAL( x.VALUE, y.PROF ) )
                return (AddMutualToEqualsReturnTrue( x, y ));

            if ( EQUAL( x.PRE_PROF, y.VALUE ) || EQUAL( x.VALUE, y.PRE_PROF ) )
                return (AddMutualToEqualsReturnTrue( x, y ));
           
            return (false);
        }
        #endregion 
    
        #region [.IComparer< essence >.]
        public override int Compare( essence x, essence y )
        {
            if ( object.ReferenceEquals( x, y ) )
                return (0);

            var d = x.EntityType - y.EntityType;
            if ( d != 0 ) return (d);

            d = getlen_NAME( y ) - getlen_NAME( x );
            if (d != 0) return (d);

            d = getlen_PROF( y ) - getlen_PROF( x );
            if (d != 0) return (d);

            d = getlen_VALUE( y ) - getlen_VALUE( x );
            if (d != 0) return (d);

            return (0);
        }

        private static int getlen_NAME( essence e )
        {
            if ( !e.NAME.IsEmptyOrNull() )
                return (e.NAME.Length);
            return (0);
        }
        #endregion

        private string GetBestName( essence e )
        {
            if ( !e.NAME.IsEmptyOrNull() )
                return (e.NAME);

            if ( !e.PROF.IsEmptyOrNull() )
                return (e.PROF);

            return (e.VALUE);
        }
        public override string GetDominantName( essence[] essences )
        {
            return (GetBestName( essences[ 0 ] ));
        }
    }

    /// <summary>
    /// 
    /// </summary>
    internal sealed class PlaceGroupEssenceComparer : GroupEssenceComparerBase
    {
        #region [.IEqualityComparer< TypeAttributeValue >.]
        public static readonly PlaceGroupEssenceComparer Instatnce = new PlaceGroupEssenceComparer();
        protected PlaceGroupEssenceComparer() { }

        protected override bool EqualsInternal( essence x, essence y )
        {
            /*if ( IsAncestorsDescendants( x, y ) )
            {
                return (false);
            }*/

            if ( EQUAL( x.VALUE, y.VALUE ) )
                return (AddMutualToEqualsReturnTrue( x, y ));
           
            return (false);
        }
        #endregion 
    
        #region [.IComparer< essence >.]
        public override int Compare( essence x, essence y )
        {
            if ( object.ReferenceEquals( x, y ) )
                return (0);

            var d = x.EntityType - y.EntityType;
            if ( d != 0 ) return (d);

            d = getlen_VALUE( y ) - getlen_VALUE( x );
            if (d != 0) return (d);

            return (0);
        }
        #endregion

        private string GetBestName( essence e )
        {
            return (e.VALUE);
        }
        public override string GetDominantName( essence[] essences )
        {
            return (GetBestName( essences[ 0 ] ));
        }
    }

    /// <summary>
    /// 
    /// </summary>
    internal sealed class OrgAbbrGroupEssenceComparer : GroupEssenceComparerBase
    {
        #region [.IEqualityComparer< TypeAttributeValue >.]
        public static readonly OrgAbbrGroupEssenceComparer Instatnce = new OrgAbbrGroupEssenceComparer();
        private OrgAbbrGroupEssenceComparer() { }

        protected override bool EqualsInternal( essence x, essence y )
        {
            /*if ( IsAncestorsDescendants( x, y ) )
            {
                return (false);
            }*/

            if ( EQUAL( x.VALUE, y.VALUE ) )
                return (AddMutualToEqualsReturnTrue( x, y ));
           
            return (false);
        }
        #endregion 
    
        #region [.IComparer< essence >.]
        public override int Compare( essence x, essence y )
        {
            if ( object.ReferenceEquals( x, y ) )
                return (0);

            var d = x.EntityType - y.EntityType;
            if ( d != 0 ) return (d);

            d = getlen_NAME( y ) - getlen_NAME( x );
            if (d != 0) return (d);

            d = getlen_VALUE( y ) - getlen_VALUE( x );
            if (d != 0) return (d);

            return (0);
        }

        private static int getlen_NAME( essence e )
        {
            if ( !e.NAME.IsEmptyOrNull() )
                return (e.NAME.Length);
            return (0);
        }
        #endregion

        private string GetBestName( essence e )
        {
            if ( !e.NAME.IsEmptyOrNull() )
                return (e.NAME);

            return (e.VALUE);
        }
        public override string GetDominantName( essence[] essences )
        {
            return (GetBestName( essences[ 0 ] ));
        }
    }

    /// <summary>
    /// 
    /// </summary>
    internal class ProductGroupEssenceComparer : OrgGroupEssenceComparer
    {
        new public static readonly ProductGroupEssenceComparer Instatnce = new ProductGroupEssenceComparer();
        protected ProductGroupEssenceComparer() { }

        #region [.IComparer< essence >.]
        public override int Compare( essence x, essence y )
        {
            if ( object.ReferenceEquals( x, y ) )
                return (0);

            var d = x.EntityType - y.EntityType;
            if ( d != 0 ) return (d);

            d = getlen_VALUE( y ) - getlen_VALUE( x );
            if (d != 0) return (d);

            return (0);
        }
        #endregion

        private string GetBestName( essence e )
        {
            return (e.VALUE);
        }
        public override string GetDominantName( essence[] essences )
        {
            return (GetBestName( essences[ 0 ] ));
        }
    }

    /// <summary>
    /// 
    /// </summary>
    internal class EventGroupEssenceComparer : ProductGroupEssenceComparer
    {
        new public static readonly EventGroupEssenceComparer Instatnce = new EventGroupEssenceComparer();
        private EventGroupEssenceComparer() { }
    }

    /// <summary>
    /// 
    /// </summary>
#if DEBUG
    public
#else
    internal
#endif
    static class SynsetDominantNameRetriever
    {
        internal static string Retrieve( essence[] essences )
        {
            System.Diagnostics.Debug.Assert( essences != null && essences.Any() );

            var groupType = essences[ 0 ].GroupType;
            switch ( groupType )
            {
                case CoreferenceGroupType.PersonNameGroup:
                    return (PersonNameGroupEssenceComparer.Instatnce.GetDominantName( essences ));

                case CoreferenceGroupType.PersonInfoGroup:
                    return (PersonInfoGroupEssenceComparer.Instatnce.GetDominantName( essences ));

                case CoreferenceGroupType.PersonNoInfoGroup:
                    return (PersonNoInfoGroupEssenceComparer.Instatnce.GetDominantName( essences ));

                case CoreferenceGroupType.OrgGroup:
                    return (OrgGroupEssenceComparer.Instatnce.GetDominantName( essences ));

                case CoreferenceGroupType.PlaceGroup:
                    return (PlaceGroupEssenceComparer.Instatnce.GetDominantName( essences ));

                case CoreferenceGroupType.ProductGroup:
                    return (ProductGroupEssenceComparer.Instatnce.GetDominantName( essences ));

                case CoreferenceGroupType.EventGroup:
                    return (EventGroupEssenceComparer.Instatnce.GetDominantName( essences ));

                case CoreferenceGroupType.OrgAbbrGroup:
                    return (OrgAbbrGroupEssenceComparer.Instatnce.GetDominantName( essences ));

                default:
                    throw (new ArgumentException(groupType.ToString()));
            }
        }

        public static string TryRetrieve( XElement entity )
        {
            var type = entity.GetAttributeTYPESAVEDorTYPE();
            if ( type.HasValue )
            {
                var groupType = type.Value.ToGroupType();
                if ( groupType.HasValue )
                {
                    return (Retrieve( new[] { new essence( entity, type.Value, groupType.Value ) } ));
                }
            }
            return (null);
        }
    }
}
