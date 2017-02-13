using System;
using System.Collections.Concurrent;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Threading.Tasks;
using System.Xml.Linq;
using System.Xml.XPath;

using OffsetsBases.TonalityMarking;
using TextMining.Core;

namespace TonalityMarking.Blogs
{
    /// <summary>
    /// 
    /// </summary>
    internal static class BlogsConsts
    {
        public  const string POSTS_LVL_0_XPATH  = "posts/post[ lvl = '0' ]";            
        public  const string POSTS_FORMAT_XPATH = "posts/post[ lvl = '{0}' and parent = '{1}' ]";
        public  const string ID                 = "id";
        public  const string BODY               = "body";
        //public  const string PARENT             = "parent";
        //public  const string LEVEL              = "lvl";
    }

    /// <summary>
    /// 
    /// </summary>
    internal static class BlogsProcessing
    {
        public static TonalityMarkingOutputResult4Blogs Processing( ITonalityMarking tonalityMarking, TonalityMarkingInputParams4Blogs inputParams )
        {
            tonalityMarking.ThrowIfNull("tonalityMarking");
            inputParams    .ThrowIfNull("inputParams");

            switch ( inputParams.ProcessingBlogsMode )
            {
                case ProcessingBlogsMode.Parallel:
                    return (new ParallelBlogsProcessing( tonalityMarking ).Processing( inputParams ));
                case ProcessingBlogsMode.Consecutively:
                    return (new ConsecutivelyBlogsProcessing( tonalityMarking ).Processing( inputParams ));
                default: 
                    throw (new ArgumentException(inputParams.ProcessingBlogsMode.ToString()));
            }
        }
    }

    #region [.Parallel.]
    internal class PostProcessingStructureTPL
    {
        public PostProcessingStructureTPL( string xpath4SelectPosts = BlogsConsts.POSTS_LVL_0_XPATH, int level = 0 )
        {
            xpath4SelectPosts.ThrowIfEmptyOrNull("xpath4SelectPosts");

            Xpath4SelectPosts = xpath4SelectPosts;
            Level             = level;
        }
        public PostProcessingStructureTPL( PostProcessingStructureTPL pps, string parent )
            : this( BlogsConsts.POSTS_FORMAT_XPATH.FormatEx( pps.Level + 1, parent ), pps.Level + 1 )
        {
        }

        public string Xpath4SelectPosts
        {
            get;
            private set;
        }
        public int    Level
        {
            get;
            private set;
        }

        public override string ToString()
        {
            return (Xpath4SelectPosts + ", level: " + Level);
        }
    }
    internal class BlogsProcessingStructureTPL
    {
        public BlogsProcessingStructureTPL( TonalityMarkingInputParams4Blogs inputParams4Blogs )
        {
            inputParams4Blogs.ThrowIfNull("inputParams4Blogs");

            InputParams4Blogs = inputParams4Blogs;
            Tuples            = new BlockingCollection< TonalityMarkingTuple4Blogs >();
        }

        public TonalityMarkingInputParams4Blogs InputParams4Blogs
        {
            get;
            private set;
        }
        public BlockingCollection< TonalityMarkingTuple4Blogs > Tuples
        {
            get;
            private set;
        }

        public TonalityMarkingInputParams CreateTMInputParamsYesInquiries( string inputText )
        {
            var tmInputParamsWithInquiriesSynonyms = new TonalityMarkingInputParams( inputText ) 
            { 
                InquiriesSynonyms = InputParams4Blogs.InquiriesSynonyms.ToList(),
                LanguageType      = InputParams4Blogs.LanguageType,
                ThemeType         = InputParams4Blogs.ThemeType,
                //OutputXmlFormat   = OutputXmlFormat.Full,
            };
            return (tmInputParamsWithInquiriesSynonyms);
        }
        public TonalityMarkingInputParams CreateTMInputParamsNoInquiries( TonalityMarkingOutputResult tmResult )
        {
            var tmInputParamsWithoutInquiriesSynonyms = new TonalityMarkingInputParams( tmResult.OutputXml ) 
            { 
                InquiriesSynonyms = null,
                InputTextFormat   = InputTextFormat.LinguisticXml,
                LanguageType      = InputParams4Blogs.LanguageType,
                ThemeType         = InputParams4Blogs.ThemeType,
                //OutputXmlFormat   = OutputXmlFormat.Full,
            };
            return (tmInputParamsWithoutInquiriesSynonyms);
        }

        public override string ToString()
        {
            return ("tuples: " + Tuples.Count.ToString());
        }
    }
    internal class PairTPL 
    {
        private PairTPL( BlogsProcessingStructureTPL _bps, PostProcessingStructureTPL _pps )
        {
            bps  = _bps;
            pps  = _pps;
        }

        public BlogsProcessingStructureTPL bps 
        {
            get;
            private set;
        }
        public PostProcessingStructureTPL  pps
        {
            get;
            private set;
        }

        public PairTPL Create( string parent )
        {
            var _pps = new PostProcessingStructureTPL( this.pps, parent );
            return (new PairTPL( this.bps, _pps ));
        }
        public static PairTPL Create( BlogsProcessingStructureTPL _bps )
        {
            var _pps = new PostProcessingStructureTPL();
            return (new PairTPL( _bps, _pps ));
        }
        /*public PairTPL CreateCopy()
        {
            return (new PairTPL( bps, pps ));
        }*/

        public override string ToString()
        {
            return (bps.ToString() + ", " + pps.ToString());
        }
    }

    internal class ParallelBlogsProcessing
    {
        #region [.ctor().]
        private ITonalityMarking _TonalityMarking;

        public ParallelBlogsProcessing( ITonalityMarking tonalityMarking )
        {
            tonalityMarking.ThrowIfNull("tonalityMarking");

            _TonalityMarking = tonalityMarking;
        }
        #endregion

        public TonalityMarkingOutputResult4Blogs Processing( TonalityMarkingInputParams4Blogs inputParams )
        {
            //1.
            var bps = new BlogsProcessingStructureTPL( inputParams );            

            //2.
            ProcessingRecurrent( PairTPL.Create( bps ) );

            //3.
            var result = new TonalityMarkingOutputResult4Blogs
                (
                bps.Tuples,
                RuleBase.GetOperateRulesNamesDebugInfoOutput()
                );
            return (result);
        }

        private void ProcessingRecurrent( PairTPL p )
        {
            if ( p == null )
                Debugger.Break();

            var posts = ((p.pps.Level == 0)
                        ? p.bps.InputParams4Blogs.InputXml.GetMultiNodes      ( p.pps.Xpath4SelectPosts )
                        : p.bps.InputParams4Blogs.InputXml.XPathSelectElements( p.pps.Xpath4SelectPosts )).ToArray();

            Parallel.ForEach
            (
                posts,
                () => p, //p.CreateCopy,
                PostProcessing,
                (_) => { }
            );
        }

        private PairTPL PostProcessing( XElement post, ParallelLoopState loopstate, PairTPL p )
        {
            if ( p == null )
                Debugger.Break();

            //1. - paralell
            var body = post.GetSingleNode( BlogsConsts.BODY ).Value;
            if ( body.IsEmptyOrNull() )
            {
                return (p);
            }

            var continueProcessing = true;
            var tmResult = _TonalityMarking.ExecuteTonalityMarking( p.bps.CreateTMInputParamsYesInquiries( body ) );
            if ( tmResult.InquiryAllocateStatus == InquiryAllocateStatus.NotAllocated )
            {
                if ( p.pps.Level == 0 )
                {
                    return (p);
                }

                tmResult = _TonalityMarking.ExecuteTonalityMarking( p.bps.CreateTMInputParamsNoInquiries( tmResult ) );
                if ( tmResult.InquiryAllocateStatus == InquiryAllocateStatus.NotAllocated )
                {
                    return (p);
                }
                continueProcessing = false;
            }

            //2.
            var outputXml = default(string);
            switch ( p.bps.InputParams4Blogs.OutputBlogsFormat )
            {
                case OutputBlogsFormat.LinguisticXml:
                        outputXml = tmResult.OutputXml;
                    break;
                case OutputBlogsFormat.Offsets:       
                        var offsetsProcessor = new OffsetsProcessor( /*body*/ );
                        var offsets = offsetsProcessor.CreateOffsetsString( tmResult.GetOutputXDocument().Root );
                        outputXml = offsets;
                    break;
                default: throw (new ArgumentException(p.bps.InputParams4Blogs.OutputBlogsFormat.ToString()));
            }

            //3.
            var id = post.GetSingleNode( BlogsConsts.ID ).Value;
            //var parent = post.GetSingleNode( BlogsConsts.PARENT ).Value;
            //var level  = post.GetSingleNode( BlogsConsts.LEVEL  ).Value;
            var tuple = new TonalityMarkingTuple4Blogs
                (
                id,
                outputXml,
                p.bps.InputParams4Blogs.OutputBlogsFormat,
                tmResult.AuthorTonalityMarkingTuple.Positive,
                tmResult.AuthorTonalityMarkingTuple.Negative,
                tmResult.AuthorTonalityMarkingTuple.Objects //GetOutputXmlXDocument().ExtractObjects()
                );
            p.bps.Tuples.Add( tuple );

            //4.
            if ( continueProcessing )
            {
                ProcessingRecurrent( p.Create( id ) );
            }

            return (p);
        }
    }
    #endregion

    #region [.Consecutively.]
    internal class BlogsProcessingStructure
    {
        private const string DUMMY = "DUMMY";

        public BlogsProcessingStructure( TonalityMarkingInputParams4Blogs inputParams4Blogs )
        {
            inputParams4Blogs.ThrowIfNull("inputParams4Blogs");

            InputParams4Blogs = inputParams4Blogs;
            TMInputParamsWithInquiriesSynonyms = new TonalityMarkingInputParams( DUMMY ) 
            { 
                InquiriesSynonyms = inputParams4Blogs.InquiriesSynonyms.ToList(),
                InputTextFormat   = InputTextFormat.PlainText,
                //OutputXmlFormat   = OutputXmlFormat.Full,
                LanguageType      = inputParams4Blogs.LanguageType,
                ThemeType         = inputParams4Blogs.ThemeType,
            };
            TMInputParamsWithoutInquiriesSynonyms = new TonalityMarkingInputParams( DUMMY ) 
            { 
                InquiriesSynonyms = null,
                InputTextFormat   = InputTextFormat.LinguisticXml,
                //OutputXmlFormat   = OutputXmlFormat.Full,
                LanguageType      = inputParams4Blogs.LanguageType,
                ThemeType         = inputParams4Blogs.ThemeType,
            };
            Tuples            = new List< TonalityMarkingTuple4Blogs >();
            Xpath4SelectPosts = BlogsConsts.POSTS_LVL_0_XPATH;
        }

        public TonalityMarkingInputParams4Blogs InputParams4Blogs
        {
            get;
            private set;
        }
        public TonalityMarkingInputParams TMInputParamsWithInquiriesSynonyms
        {
            get;
            private set;
        }
        public TonalityMarkingInputParams TMInputParamsWithoutInquiriesSynonyms
        {
            get;
            private set;
        }
        public List< TonalityMarkingTuple4Blogs > Tuples
        {
            get;
            private set;
        }
        public string Xpath4SelectPosts
        {
            get;
            set;
        }
    }

    internal class ConsecutivelyBlogsProcessing
    {
        #region [.ctor().]
        private ITonalityMarking _TonalityMarking;

        public ConsecutivelyBlogsProcessing( ITonalityMarking tonalityMarking )
        {
            tonalityMarking.ThrowIfNull("tonalityMarking");

            _TonalityMarking = tonalityMarking;
        }
        #endregion

        public TonalityMarkingOutputResult4Blogs Processing( TonalityMarkingInputParams4Blogs inputParams )
        {
            //1.
            var bps = new BlogsProcessingStructure( inputParams );

            //2.
            ProcessingRecurrent( bps );

            //3.
            var result = new TonalityMarkingOutputResult4Blogs
                (
                bps.Tuples,
                RuleBase.GetOperateRulesNamesDebugInfoOutput()
                );
            return (result);
        }

        private void ProcessingRecurrent( BlogsProcessingStructure bps, int level = 0 )
        {
            var posts = ((level == 0)
                        ? bps.InputParams4Blogs.InputXml.GetMultiNodes      ( bps.Xpath4SelectPosts )
                        : bps.InputParams4Blogs.InputXml.XPathSelectElements( bps.Xpath4SelectPosts )).ToArray(); 
            foreach ( var post in posts )
            {
                //1.
                var body = post.GetSingleNode( BlogsConsts.BODY ).Value;
                if ( body.IsEmptyOrNull() )
                {
                    continue;
                }

                bps.TMInputParamsWithInquiriesSynonyms.InputText = body;

                var continueProcessing = true;
                var tmResult = _TonalityMarking.ExecuteTonalityMarking( bps.TMInputParamsWithInquiriesSynonyms );
                if ( tmResult.InquiryAllocateStatus == InquiryAllocateStatus.NotAllocated )
                {
                    if ( level == 0 )
                    {
                        continue;
                    }

                    bps.TMInputParamsWithoutInquiriesSynonyms.InputText = tmResult.OutputXml;

                    tmResult = _TonalityMarking.ExecuteTonalityMarking( bps.TMInputParamsWithoutInquiriesSynonyms );
                    if ( tmResult.InquiryAllocateStatus == InquiryAllocateStatus.NotAllocated )
                    {
                        continue;
                    }
                    continueProcessing = false;
                }

                //2.
                var outputXml = default(string);
                switch ( bps.InputParams4Blogs.OutputBlogsFormat )
                {
                    case OutputBlogsFormat.LinguisticXml:
                            outputXml = tmResult.OutputXml;
                        break;
                    case OutputBlogsFormat.Offsets:       
                            var offsetsProcessor = new OffsetsProcessor( /*body*/ );
                            var offsets = offsetsProcessor.CreateOffsetsString( tmResult.GetOutputXDocument().Root );
                            outputXml = offsets;
                        break;
                    default: throw (new ArgumentException(bps.InputParams4Blogs.OutputBlogsFormat.ToString()));
                }

                //3.
                var id = post.GetSingleNode( BlogsConsts.ID ).Value;
                //var parent = post.GetSingleNode( BlogsConsts.PARENT ).Value;
                //var level  = post.GetSingleNode( BlogsConsts.LEVEL  ).Value;

                var tuple = new TonalityMarkingTuple4Blogs
                    (
                    id,
                    outputXml,
                    bps.InputParams4Blogs.OutputBlogsFormat,
                    tmResult.AuthorTonalityMarkingTuple.Positive,
                    tmResult.AuthorTonalityMarkingTuple.Negative,
                    tmResult.AuthorTonalityMarkingTuple.Objects //GetOutputXmlXDocument().ExtractObjects()
                    );
                bps.Tuples.Add( tuple );

                //4.
                if ( continueProcessing )
                {
                    bps.Xpath4SelectPosts = BlogsConsts.POSTS_FORMAT_XPATH.FormatEx( level + 1, id );
                    ProcessingRecurrent( bps, level + 1 );
                }
            }
        }
    }
    #endregion
}